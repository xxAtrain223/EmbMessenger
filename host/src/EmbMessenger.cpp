#include "EmbMessenger/EmbMessenger.hpp"

namespace emb
{
    EmbMessenger::EmbMessenger(IBuffer* buffer) :
        m_buffer(buffer),
        m_writer(buffer),
        m_reader(buffer)
    {
        m_message_id = 1;

        registerCommand<ResetCommand>(255);
        registerCommand<RegisterPeriodicCommand>(254);
        registerCommand<UnregisterPeriodicCommand>(253);
    }

    std::shared_ptr<Command> EmbMessenger::send(std::shared_ptr<Command> command)
    {
        if (command->getTypeIndex() == typeid(Command))
        {
            throw InvalidCommandTypeIndexHostException("Command has an invalid type index, override it in your command derived class.", command);
        }

        command->m_message_id = m_message_id;
        m_commands.emplace(m_message_id, command);

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
            m_current_command = m_commands.at(message_id);
        }
        catch (std::out_of_range e)
        {
            consumeMessage();
            m_commands.erase(message_id);
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
            m_commands.erase(message_id);
            throw;
        }

        if (m_reader.nextCrc())
        {
            if (!m_reader.readCrc())
            {
                m_commands.erase(message_id);
                throw CrcInvalidHostException("Crc from the device was invalid", m_current_command);
            }
        }
        else
        {
            consumeMessage();
            m_commands.erase(message_id);
            throw ExtraParametersHostException("Message has extra parameters from the device", m_current_command);
        }

        if (m_current_command->m_callback != nullptr)
        {
            m_current_command->m_callback(m_current_command);
        }

        m_current_command = nullptr;

        if (!m_commands.at(message_id)->m_is_periodic)
        {
            m_commands.erase(message_id);
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
            m_reader.readError(error);
            
            switch (error)
            {
            case DataError::kExtraParameters:
                throw ExtraParametersDeviceException("The device received one or more extra parameters", m_current_command);
            case DataError::kOutOfPeriodicCommandSlots:
                throw OutOfPeriodicCommandSlotsDeviceException("The device ran out of periodic command slots", m_current_command);
            case DataError::kParameter0ReadError:
            case DataError::kParameter1ReadError:
            case DataError::kParameter2ReadError:
            case DataError::kParameter3ReadError:
            case DataError::kParameter4ReadError:
            case DataError::kParameter5ReadError:
            case DataError::kParameter6ReadError:
            case DataError::kParameter7ReadError:
                throw ParameterReadErrorDeviceException(error - DataError::kParameter0ReadError, m_current_command);
            case DataError::kMessageIdReadError:
                throw MessageIdReadErrorDeviceException("The device encountered an error reading the message id", m_current_command);
            case DataError::kCommandIdReadError:
                throw CommandIdReadErrorDeviceException("The device encountered an error reading the command id", m_current_command);
            case DataError::kCrcReadError:
                throw CrcReadErrorDeviceException("The device encountered an error reading the CRC", m_current_command);
            case DataError::kParameter0Invalid:
            case DataError::kParameter1Invalid:
            case DataError::kParameter2Invalid:
            case DataError::kParameter3Invalid:
            case DataError::kParameter4Invalid:
            case DataError::kParameter5Invalid:
            case DataError::kParameter6Invalid:
            case DataError::kParameter7Invalid:
                throw ParameterInvalidDeviceException(error - DataError::kParameter0Invalid, m_current_command);
            case DataError::kMessageIdInvalid:
                throw MessageIdInvalidDeviceException("The device read an invalid message id", m_current_command);
            case DataError::kCommandIdInvalid:
                throw CommandIdInvalidDeviceException("The device read an invalid command id", m_current_command);
            case DataError::kCrcInvalid:
                throw CrcInvalidDeviceException("The device read an invalid CRC", m_current_command);
            default:
                m_current_command->reportError(error, m_current_command);
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
        messenger->write(m_message_id);
    }

    void EmbMessenger::UnregisterPeriodicCommand::receive(EmbMessenger* messenger)
    {
        messenger->read(m_periodic_message_id);
    }

    void EmbMessenger::resetDevice()
    {
        std::shared_ptr<ResetCommand> resetCommand = std::make_shared<ResetCommand>();
        send(resetCommand);
        // TODO: Wait for resetCommand to receive acknoledgement
    }
}