#ifndef EMBMESSENGER_EMBMESSENGER_HPP
#define EMBMESSENGER_EMBMESSENGER_HPP

#include <cstdint>
#include <csetjmp>

#include "EmbMessenger/Reader.hpp"
#include "EmbMessenger/Writer.hpp"
#include "EmbMessenger/DataError.hpp"

#ifndef EMB_TESTING
#else
#include <functional>
#endif

namespace emb
{
    class IBuffer;

    template <uint8_t MaxCommands, uint8_t MaxPeriodicCommands>
    class EmbMessenger
    {
    protected:
#ifndef EMB_TESTING
        using CommandFunction = (void)(*)(void);
#else
        using CommandFunction = std::function<void()>;
#endif

        IBuffer* m_buffer;
        Reader m_reader;
        Writer m_writer;
        jmp_buf m_jmp_buf;

        CommandFunction m_commands[MaxCommands];

        uint8_t m_command_id;
        uint16_t m_message_id;
        bool m_is_periodic;
        bool m_in_command;
        uint8_t m_num_messages;

        uint8_t m_parameter_index;

        void read()
        {
            if (m_reader.nextCrc() && !m_reader.readCrc())
            {
                reportError(DataError::kCrcInvalid);
            }
        }

        void read_and_validate()
        {
            (void)0;
        }

        void write()
        {
            (void)0; // Noop
        }

        void consumeMessage()
        {
            for (uint8_t n = m_buffer->messagesAvailable();
                n > 0 && n == m_buffer->messagesAvailable() && n == m_num_messages;
                m_buffer->readByte());
        }

    public:
        EmbMessenger(IBuffer* buffer) :
            m_buffer(buffer),
            m_reader(buffer),
            m_writer(buffer)
        {
            static_assert(MaxCommands < 0xF0, "MaxCommands must be less than 0xF0 (240)");
            static_assert(MaxPeriodicCommands < 0xF0, "MaxPeriodicCommands must be less than 0xF0 (240)");

            for (uint8_t i = 0; i < MaxCommands; ++i)
            {
                m_commands[i] = nullptr;
            }

            m_in_command = false;
        }

        bool attachCommand(int id, CommandFunction command)
        {
            if (id >= MaxCommands)
            {
                return false;
            }

            if (m_commands[id] != nullptr)
            {
                return false;
            }

            m_commands[id] = command;
            return true;
        }

        uint8_t getCommandId() const
        {
            return m_command_id;
        }

        uint16_t getMessageId() const
        {
            return m_message_id;
        }

        bool getIsPeriodic() const
        {
            return m_is_periodic;
        }

        void update()
        {
            m_buffer->update();
            m_num_messages = m_buffer->messagesAvailable();

            if (m_num_messages == 0)
            {
                return;
            }

            m_reader.resetCrc();
            m_message_id = 0;
            m_parameter_index = 0;

            if (!m_reader.read(m_message_id))
            {
                m_writer.writeError(DataError::kMessageIdReadError);
                consumeMessage();
                m_writer.writeCrc();
                return;
            }
            m_writer.write(m_message_id);

            if (!m_reader.read(m_command_id))
            {
                m_writer.writeError(DataError::kCommandIdReadError);
                consumeMessage();
                m_writer.writeCrc();
                return;
            }

            switch (m_command_id)
            {
            default:
                if (m_commands[m_command_id] == nullptr)
                {
                    m_writer.writeError(DataError::kCommandIdInvalid);
                    consumeMessage();
                }
                else
                {
                    if (setjmp(m_jmp_buf) == 0)
                    {
                        m_in_command = true;
                        m_commands[m_command_id]();
                        m_in_command = false;
                    }
                    else
                    {
                        consumeMessage();
                        m_writer.writeCrc();
                        return;
                    }
                }
            }

            if (m_buffer->messagesAvailable() == m_num_messages)
            {
                if (m_reader.nextCrc())
                {
                    if (!m_reader.readCrc())
                    {
                        consumeMessage();
                        m_writer.writeError(DataError::kCrcInvalid);
                    }
                }
                else
                {
                    consumeMessage();
                    m_writer.writeError(DataError::kExtraParameters);
                }
            }

            m_writer.writeCrc();
        }

        template <typename T, typename... Ts>
        void read(T&& value, Ts&&... args)
        {
            if (!m_in_command)
            {
                return;
            }

            if (!m_reader.read(value))
            {
                reportError(static_cast<DataError>(DataError::kParameter0ReadError + m_parameter_index));
            }
            
            ++m_parameter_index;

            read(std::forward<Ts>(args)...);
        }
        
        template <typename T, typename... Ts>
        void read_and_validate(T&& value, std::function<bool(decltype(value))>&& validator, Ts&&... args)
        {
            if (!m_in_command)
            {
                return;
            }

            read(value);

            if (!validator(value))
            {
                reportError(static_cast<DataError>(DataError::kParameter0Invalid + m_parameter_index - 1u));
            }

            read_and_validate(std::forward<Ts>(args)...);
        }

        template <typename T, typename... Ts>
        void write(const T value, const Ts... args)
        {
            if (!m_in_command)
            {
                return;
            }

            m_writer.write(value);

            write(args...);
        }

        void reportError(const DataError code)
        {
            if (!m_in_command)
            {
                return;
            }

            m_writer.writeError(code);
            
            longjmp(m_jmp_buf, code);
        }
    };
}

#endif // EMBMESSENGER_EMBMESSENGER_HPP
