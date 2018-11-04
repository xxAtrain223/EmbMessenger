#include "EmbMessenger/EmbMessenger.hpp"
#include <iostream>
#include <chrono>
#include <thread>

namespace emb
{
#ifdef EMB_SINGLE_THREADED
    EmbMessenger::EmbMessenger(IBuffer* buffer, std::chrono::milliseconds init_timeout) :
#else
    EmbMessenger::EmbMessenger(IBuffer* buffer, std::function<bool(std::exception_ptr)> exception_handler, std::chrono::milliseconds init_timeout) :
        m_exception_handler(exception_handler),
#endif
        m_buffer(buffer),
        m_writer(buffer),
        m_reader(buffer)
    {
        m_message_id = 0;

        registerCommand<ResetCommand>(255);
        registerCommand<RegisterPeriodicCommand>(254);
        registerCommand<UnregisterPeriodicCommand>(253);

        using clock_t = std::chrono::steady_clock;
        bool initializing = true;
        auto initializingEnd = clock_t::now() + init_timeout;

        while (clock_t::now() < initializingEnd && initializing)
        {
            auto resetCommand = std::make_shared<ResetCommand>();
            resetCommand->setCallback<ResetCommand>([&](auto&& cmd) {
                initializing = false;
            });
            send(resetCommand);

            auto end = clock_t::now() + std::chrono::milliseconds(500);
            if (end > initializingEnd)
            {
                end = initializingEnd;
            }

            while (clock_t::now() < end && initializing)
            {
                try
                {
                    update();
                }
                catch (BaseException e)
                { }
            }
        }

        if (!initializing)
        {
            m_commands.clear();
            m_buffer->zero();
        }
        else
        {
            throw InitializingErrorHostException("Timed out initializing connection");
        }

#ifndef EMB_SINGLE_THREADED
        m_update_thread = std::thread(&EmbMessenger::updateThread, this);
#endif
    }

#ifndef EMB_SINGLE_THREADED
    EmbMessenger::~EmbMessenger()
    {
        m_running = false;
        m_update_thread.join();
    }

    bool EmbMessenger::running() const
    {
        return m_running;
    }

    void EmbMessenger::stop()
    {
        m_running = false;
    }

    void EmbMessenger::updateThread()
    {
        m_running = true;

        while (m_running)
        {
            try
            {
                update();
            }
            catch (...)
            {
                if (m_exception_handler)
                {
                    m_running = !m_exception_handler(std::current_exception());
                }
            }
        }
    }
#endif

    std::shared_ptr<Command> EmbMessenger::send(std::shared_ptr<Command> command)
    {
        if (command->getTypeIndex() == typeid(Command))
        {
            throw InvalidCommandTypeIndexHostException("Command has an invalid type index, override it in your command derived class.", command);
        }

        command->m_message_id = m_message_id;
        {
            #ifndef EMB_SINGLE_THREADED
            std::lock_guard<std::mutex> lock(m_commands_mutex);
            #endif
            m_commands.emplace(m_message_id, command);
        }

        write(m_message_id++, m_command_ids.at(command->getTypeIndex()));
        command->send(this);
        m_writer.writeCrc();

        return command;
    }

    void EmbMessenger::update()
    {
        m_buffer->update();

        if (m_buffer->messagesAvailable() == 0)
        {
            return;
        }

        m_reader.resetCrc();

        try
        {
            readErrors();
        }
        catch (...)
        {
            consumeMessage();
            throw;
        }

        uint16_t message_id = 0;
        if (!m_reader.read(message_id))
        {
            consumeMessage();
            throw MessageIdReadErrorHostException("Error reading message Id");
        }

        try
        {
            #ifndef EMB_SINGLE_THREADED
            std::lock_guard<std::mutex> lock(m_commands_mutex);
            #endif
            m_current_command = m_commands.at(message_id);
        }
        catch (std::out_of_range e)
        {
            consumeMessage();
            throw MessageIdInvalidHostException("No command to receive message from the device", m_current_command);
        }

        m_parameter_index = 0;
        try
        {
            m_current_command->receive(this);
        }
        catch (...)
        {
            consumeMessage();
            #ifndef EMB_SINGLE_THREADED
            std::lock_guard<std::mutex> lock(m_commands_mutex);
            #endif
            m_commands.erase(message_id);
            throw;
        }

        try
        {
            readErrors();
        }
        catch (...)
        {
            consumeMessage();
            #ifndef EMB_SINGLE_THREADED
            std::lock_guard<std::mutex> lock(m_commands_mutex);
            #endif
            m_commands.erase(message_id);
            throw;
        }

        if (m_reader.nextCrc())
        {
            if (!m_reader.readCrc())
            {
                #ifndef EMB_SINGLE_THREADED
                std::lock_guard<std::mutex> lock(m_commands_mutex);
                #endif
                m_commands.erase(message_id);
                throw CrcInvalidHostException("Crc from the device was invalid", m_current_command);
            }
        }
        else
        {
            consumeMessage();
            #ifndef EMB_SINGLE_THREADED
            std::lock_guard<std::mutex> lock(m_commands_mutex);
            #endif
            m_commands.erase(message_id);
            throw ExtraParametersHostException("Message has extra parameters from the device", m_current_command);
        }

        #ifndef EMB_SINGLE_THREADED
        m_current_command->m_received = true;
        if (m_current_command->m_is_waiting)
        {
            m_current_command->m_condition_variable.notify_all();
        }
        #endif

        if (m_current_command->m_callback != nullptr)
        {
            m_current_command->m_callback(m_current_command);
        }

        m_current_command = nullptr;

        {
            #ifndef EMB_SINGLE_THREADED
            std::lock_guard<std::mutex> lock(m_commands_mutex);
            #endif
            if (!m_commands.at(message_id)->m_is_periodic)
            {
                m_commands.erase(message_id);
            }
        }
    }

    void EmbMessenger::write()
    {
        (void)0; // Noop
    }

    void EmbMessenger::read()
    {
        (void)0; // Noop
    }

    void EmbMessenger::readErrors()
    {
        while (m_reader.nextError())
        {
            uint8_t error = 0;
            int16_t data = 0;
            m_reader.readError(error);
            m_reader.read(data);
            
            switch (error)
            {
            case DataError::kExtraParameters:
                throw ExtraParametersDeviceException("The device received one or more extra parameters", m_current_command);
            case DataError::kOutOfPeriodicCommandSlots:
                throw OutOfPeriodicCommandSlotsDeviceException("The device ran out of periodic command slots", m_current_command);
            case DataError::kParameterReadError:
                throw ParameterReadErrorDeviceException(data, m_current_command);
            case DataError::kMessageIdReadError:
                throw MessageIdReadErrorDeviceException("The device encountered an error reading the message id", m_current_command);
            case DataError::kCommandIdReadError:
                throw CommandIdReadErrorDeviceException("The device encountered an error reading the command id", m_current_command);
            case DataError::kCrcReadError:
                throw CrcReadErrorDeviceException("The device encountered an error reading the CRC", m_current_command);
            case DataError::kParameterInvalid:
                throw ParameterInvalidDeviceException(data, m_current_command);
            case DataError::kMessageIdInvalid:
                throw MessageIdInvalidDeviceException("The device read an invalid message id", m_current_command);
            case DataError::kCommandIdInvalid:
                throw CommandIdInvalidDeviceException("The device read an invalid command id", m_current_command);
            case DataError::kCrcInvalid:
                throw CrcInvalidDeviceException("The device read an invalid CRC", m_current_command);
            default:
                m_current_command->reportError(error, data, m_current_command);
                throw DeviceException(error, "The device reported a user defined error.", m_current_command);
            }
        }
    }

    void EmbMessenger::consumeMessage()
    {
        for (uint8_t n = m_buffer->messagesAvailable();
             n > 0 && n == m_buffer->messagesAvailable();
             m_buffer->readByte());
    }

    EmbMessenger::ResetCommand::ResetCommand()
    {
        m_type_index = typeid(ResetCommand);
    }

    EmbMessenger::RegisterPeriodicCommand::RegisterPeriodicCommand(uint8_t commandId, uint32_t period)
        : m_command_id(commandId), m_period(period)
    {
        m_type_index = typeid(RegisterPeriodicCommand);
    }

    void EmbMessenger::RegisterPeriodicCommand::send(EmbMessenger* messenger)
    {
        messenger->write(m_command_id, m_period);
    }

    EmbMessenger::UnregisterPeriodicCommand::UnregisterPeriodicCommand(uint8_t commandId)
        : m_command_id(commandId)
    {
        m_type_index = typeid(UnregisterPeriodicCommand);
    }

    void EmbMessenger::UnregisterPeriodicCommand::send(EmbMessenger* messenger)
    {
        messenger->write(m_command_id);
    }

    void EmbMessenger::UnregisterPeriodicCommand::receive(EmbMessenger* messenger)
    {
        messenger->read(m_periodic_message_id);
    }

    void EmbMessenger::resetDevice()
    {
        std::shared_ptr<ResetCommand> resetCommand = std::make_shared<ResetCommand>();
        send(resetCommand);
        #ifndef EMB_SINGLE_THREADED
        resetCommand->wait();
        #endif
    }
}