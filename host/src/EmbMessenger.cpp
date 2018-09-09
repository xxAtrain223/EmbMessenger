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

    void EmbMessenger::update()
    {
        m_buffer->update();

        if (m_buffer->messagesAvailable() == 0)
        {
            return;
        }

        m_reader.resetCrc();

        uint16_t message_id = 0;
        if (!m_reader.read(message_id))
        {
            // TODO: Throw exception
            printf("Error reading message Id, consuming message\n");
            consumeMessage();
            return;
        }

        std::shared_ptr<Command> command = nullptr;
        try
        {
            command = m_commands.at(message_id);
        }
        catch (std::out_of_range e)
        {
            // No command to receive message, consume message
            consumeMessage();
            return;
        }

        command->receive(this);

        readErrors();

        if (m_reader.nextCrc())
        {
            if (!m_reader.readCrc())
            {
                // TODO: Throw exception
                printf("Crc Invalid\n");
            }
        }
        else
        {
            printf("Extra Parameters, consuming rest of message\n");
            consumeMessage();
        }

        if (command->m_callback != nullptr)
        {
            command->m_callback(command);
        }

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
            // TODO: Throw exception
            printf("Error: 0x%02X\n", error);
        }
    }

    void EmbMessenger::consumeMessage()
    {
        for (uint8_t n = m_buffer->messagesAvailable();
             n > 0 && n == m_buffer->messagesAvailable();
             m_buffer->readByte());
    }

    EmbMessenger::RegisterPeriodicCommand::RegisterPeriodicCommand(uint8_t commandId, uint32_t period)
        : m_command_id(commandId), m_period(period)
    {
    }

    void EmbMessenger::RegisterPeriodicCommand::send(EmbMessenger* messenger)
    {
        messenger->write(m_command_id, m_period);
    }

    EmbMessenger::UnregisterPeriodicCommand::UnregisterPeriodicCommand(uint8_t commandId)
        : m_command_id(commandId)
    {
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