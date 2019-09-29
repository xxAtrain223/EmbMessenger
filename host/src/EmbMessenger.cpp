#include "EmbMessenger/EmbMessenger.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <sstream>
#include <iomanip>

namespace emb
{
    namespace host
    {
#ifdef EMB_SINGLE_THREADED
        EmbMessenger::EmbMessenger(shared::IBuffer* buffer, std::chrono::milliseconds init_timeout) :
#else
        EmbMessenger::EmbMessenger(shared::IBuffer* buffer, std::function<bool(std::exception_ptr)> exception_handler,
                                   std::chrono::milliseconds init_timeout) :
            m_exception_handler(exception_handler),
#endif
            m_buffer(buffer),
            m_writer(buffer),
            m_reader(buffer)
        {
            m_message_id = 0;

            registerCommand<ResetCommand>(0xFFFF);
            registerCommand<RegisterPeriodicCommand>(0xFFFE);
            registerCommand<UnregisterPeriodicCommand>(0xFFFD);

            using clock_t = std::chrono::steady_clock;
            bool initializing = true;
            auto initializingEnd = clock_t::now() + init_timeout;

            while (clock_t::now() < initializingEnd && initializing)
            {
                auto resetCommand = std::make_shared<ResetCommand>();
                resetCommand->setCallback<ResetCommand>([&](auto&& cmd) { initializing = false; });
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
                    {
                    }
                }
            }

            if (!initializing)
            {
                m_commands.clear();
                m_buffer->zero();
            }
            else
            {
                throw InitializationError("Timed out initializing connection");
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
            std::type_index type_index = command->getTypeIndex();
            uint16_t command_id = 0;
            try
            {
                command_id = m_command_ids.at(type_index);
            }
            catch (const std::out_of_range& e)
            {
                throw UnregisteredCommand("The command was not registered.");
            }

            return send(command, command_id);
        }

        std::shared_ptr<Command> EmbMessenger::send(std::shared_ptr<Command> command, uint16_t command_id)
        {
            command->m_message_id = m_message_id;
            {
#ifndef EMB_SINGLE_THREADED
                std::lock_guard<std::mutex> lock(m_commands_mutex);
#endif
                m_commands.emplace(m_message_id, command);
            }

            write(m_message_id++, command_id);
            command->send(this);
            command->m_command_state = CommandState::Sent;
            m_writer.writeCrc();

            return command;
        }

        void EmbMessenger::update()
        {
            m_buffer->update();

            if (m_buffer->messages() == 0)
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
                throw MessageIdReadError(ExceptionSource::Host, "Error reading message Id");
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
                throw MessageIdInvalid("No command to receive message from the device", m_current_command);
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
                    throw CrcInvalid(ExceptionSource::Host, "Crc from the device was invalid", m_current_command);
                }
            }
            else
            {
                consumeMessage();
#ifndef EMB_SINGLE_THREADED
                std::lock_guard<std::mutex> lock(m_commands_mutex);
#endif
                m_commands.erase(message_id);
                throw ExtraParameters(ExceptionSource::Host, "Message has extra parameters from the device", m_current_command);
            }

            m_current_command->m_command_state = CommandState::Received;
#ifndef EMB_SINGLE_THREADED
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
            (void)0;  // Noop
        }

        void EmbMessenger::read()
        {
            (void)0;  // Noop
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
                    case shared::DataError::kExtraParameters:
                        throw ExtraParameters(ExceptionSource::Device, "The device received one or more extra parameters",
                                                             m_current_command);
                    case shared::DataError::kOutOfPeriodicCommandSlots:
                        throw OutOfPeriodicCommandSlots("The device ran out of periodic command slots",
                                                                       m_current_command);
                    case shared::DataError::kParameterReadError:
                        throw ParameterReadError(ExceptionSource::Device, data, m_current_command);
                    case shared::DataError::kMessageIdReadError:
                        throw MessageIdReadError(ExceptionSource::Device,
                            "The device encountered an error reading the message id", m_current_command);
                    case shared::DataError::kCommandIdReadError:
                        throw CommandIdReadError(
                            "The device encountered an error reading the command id", m_current_command);
                    case shared::DataError::kCrcReadError:
                        throw CrcReadError(ExceptionSource::Device, "The device encountered an error reading the CRC",
                                                          m_current_command);
                    case shared::DataError::kParameterInvalid:
                        throw ParameterInvalid(data, m_current_command);
                    case shared::DataError::kCommandIdInvalid:
                        throw CommandIdInvalid("The device read an invalid command id",
                                                              m_current_command);
                    case shared::DataError::kCrcInvalid:
                        throw CrcInvalid(ExceptionSource::Device, "The device read an invalid CRC", m_current_command);
                    default:
                        m_current_command->reportError(error, data, m_current_command);
                        std::stringstream stream;
                        stream << std::hex << error;
                        throw BaseException(ExceptionSource::Device, "The device reported a user defined error. 0x" + stream.str(), m_current_command);
                }
            }
        }

        void EmbMessenger::consumeMessage()
        {
            for (uint8_t n = m_buffer->messages(); 
                 n > 0 && n == m_buffer->messages(); 
                 m_buffer->readByte());
        }

        EmbMessenger::ResetCommand::ResetCommand()
        {
            m_type_index = typeid(ResetCommand);
        }

        EmbMessenger::RegisterPeriodicCommand::RegisterPeriodicCommand(uint16_t commandId, uint32_t period) :
            m_command_id(commandId),
            m_period(period)
        {
            m_type_index = typeid(RegisterPeriodicCommand);
        }

        void EmbMessenger::RegisterPeriodicCommand::send(EmbMessenger* messenger)
        {
            messenger->write(m_command_id, m_period);
        }

        EmbMessenger::UnregisterPeriodicCommand::UnregisterPeriodicCommand(uint16_t commandId) : m_command_id(commandId)
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

        bool EmbMessenger::commandsReceived()
        {
#ifndef EMB_SINGLE_THREADED
            std::lock_guard<std::mutex> lock(m_commands_mutex);
#endif
            for (const auto& x : m_commands)
            {
                if (x.second->getCommandState() != CommandState::Received)
                {
                    return false;
                }
            }

            return true;
        }

        void EmbMessenger::resetDevice()
        {
            std::shared_ptr<ResetCommand> resetCommand = std::make_shared<ResetCommand>();
            send(resetCommand);
#ifndef EMB_SINGLE_THREADED
            resetCommand->wait();
#endif
        }
    }  // namespace host
}  // namespace emb
