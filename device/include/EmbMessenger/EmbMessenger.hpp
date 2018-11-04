#ifndef EMBMESSENGER_EMBMESSENGER_HPP
#define EMBMESSENGER_EMBMESSENGER_HPP

#include <stdint.h>
#include <setjmp.h>

#include "EmbMessenger/Reader.hpp"
#include "EmbMessenger/Writer.hpp"
#include "EmbMessenger/DataError.hpp"

#ifndef EMB_TESTING
#else
#include <functional>
#endif

namespace emb
{
    namespace
    {
        template<typename T> struct remove_reference { typedef T type; };
        template<typename T> struct remove_reference<T&> { typedef T type; };
        template<typename T> struct remove_reference<T&&> { typedef T type; };
    }

    class IBuffer;

    template <uint8_t MaxCommands, uint8_t MaxPeriodicCommands>
    class EmbMessenger
    {
    protected:
#ifndef EMB_TESTING
        using CommandFunction = void(*)(void);
        using MillisFunction = uint32_t(*)(void);
#else
        using CommandFunction = std::function<void()>;
        using MillisFunction = std::function<uint32_t()>;
#endif

        struct PeriodicCommand
        {
            uint8_t command_id = 255;
            uint16_t message_id = 0;
            uint32_t millis_interval = 0;
            uint32_t next_millis = 0;
        };

        IBuffer* m_buffer;
        Reader m_reader;
        Writer m_writer;
        jmp_buf m_jmp_buf;

        uint8_t m_command_id;
        uint16_t m_message_id;
        bool m_is_periodic;
        bool m_in_command;
        uint8_t m_num_messages;

        uint16_t m_parameter_index;

        MillisFunction m_millis;

        CommandFunction m_commands[MaxCommands];
        PeriodicCommand m_periodic_commands[MaxPeriodicCommands];

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
                n > 0 && n == m_buffer->messagesAvailable() && n == m_num_messages && !m_buffer->empty();
                m_buffer->readByte());
        }

        void resetPeriodicCommands()
        {
            read();
            for (uint8_t i = 0; i < MaxPeriodicCommands; ++i)
            {
                m_periodic_commands[i].command_id = 255;
                m_periodic_commands[i].message_id = 0;
                m_periodic_commands[i].millis_interval = 0;
                m_periodic_commands[i].next_millis = 0;
            }
        }

        void registerPeriodicCommand()
        {
            uint8_t periodic_command_id;
            uint32_t periodic_interval;

            read(periodic_command_id, periodic_interval);

            if (m_commands[periodic_command_id] == nullptr)
            {
                reportError(DataError::kParameterInvalid, 0);
            }

            bool registeredCommand = false;
            for (uint8_t i = 0; i < MaxPeriodicCommands; ++i)
            {
                if (m_periodic_commands[i].command_id >= 0xF0)
                {
                    m_periodic_commands[i].command_id = periodic_command_id;
                    m_periodic_commands[i].message_id = m_message_id;
                    m_periodic_commands[i].millis_interval = periodic_interval;
                    m_periodic_commands[i].next_millis = m_millis();
                    registeredCommand = true;
                    break;
                }
            }

            if (!registeredCommand)
            {
                reportError(DataError::kOutOfPeriodicCommandSlots);
                return;
            }
        }

        void unregisterPeriodicCommand()
        {
            uint8_t periodic_command_id;

            read(periodic_command_id);

            for (uint8_t i = 0; i < MaxPeriodicCommands; ++i)
            {
                if (m_periodic_commands[i].command_id == periodic_command_id)
                {
                    write(m_periodic_commands[i].message_id);

                    m_periodic_commands[i].command_id = 255;
                    m_periodic_commands[i].message_id = 0;
                    m_periodic_commands[i].millis_interval = 0;
                    m_periodic_commands[i].next_millis = 0;
                    return;
                }
            }

            reportError(DataError::kParameterInvalid, 0);
        }

    public:
        EmbMessenger(IBuffer* buffer, MillisFunction millis = []() { return 0; }) :
            m_buffer(buffer),
            m_reader(buffer),
            m_writer(buffer),
            m_millis(millis)
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

            if (m_num_messages != 0)
            {
                m_reader.resetCrc();
                m_message_id = 0;
                m_parameter_index = 0;

                if (!m_reader.read(m_message_id))
                {
                    m_writer.writeError(DataError::kMessageIdReadError);
                    m_writer.write(0);
                    consumeMessage();
                    m_writer.writeCrc();
                    return;
                }
                m_writer.write(m_message_id);

                if (!m_reader.read(m_command_id))
                {
                    m_writer.writeError(DataError::kCommandIdReadError);
                    m_writer.write(0);
                    consumeMessage();
                    m_writer.writeCrc();
                    return;
                }

                m_in_command = true;
                if (setjmp(m_jmp_buf) == 0)
                {
                    switch (m_command_id)
                    {
                    case 0xFF:
                        resetPeriodicCommands();
                        break;
                    case 0xFE:
                        registerPeriodicCommand();
                        break;
                    case 0xFD:
                        unregisterPeriodicCommand();
                        break;
                    default:
                        if (m_command_id >= MaxCommands || m_commands[m_command_id] == nullptr)
                        {
                            m_writer.writeError(DataError::kCommandIdInvalid);
                            m_writer.write(m_command_id);
                            consumeMessage();
                        }
                        else
                        {
                            m_commands[m_command_id]();
                        }
                    }
                }
                else
                {
                    consumeMessage();
                }
                m_in_command = false;

                if (m_buffer->messagesAvailable() == m_num_messages)
                {
                    if (m_reader.nextCrc())
                    {
                        if (!m_reader.readCrc())
                        {
                            consumeMessage();
                            m_writer.writeError(DataError::kCrcInvalid);
                            m_writer.write(0);
                        }
                    }
                    else
                    {
                        consumeMessage();
                        m_writer.writeError(DataError::kExtraParameters);
                        m_writer.write(0);
                    }
                }

                m_writer.writeCrc();
            }

            m_is_periodic = true;
            uint32_t mil = m_millis();
            for (uint8_t i = 0; i < MaxPeriodicCommands; ++i)
            {
                if (m_periodic_commands[i].command_id < 0xF0 &&
                    m_periodic_commands[i].next_millis <= mil)
                {
                    m_command_id = m_periodic_commands[i].command_id;
                    m_message_id = m_periodic_commands[i].message_id;
                    m_parameter_index = 0;

                    m_writer.write(m_message_id);
                    m_in_command = true;
                    if (setjmp(m_jmp_buf) == 0)
                    {
                        m_commands[m_command_id]();
                    }
                    m_in_command = false;
                    m_writer.writeCrc();
                    m_periodic_commands[i].next_millis += m_periodic_commands[i].millis_interval;
                    mil = m_millis();
                }
            }
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
                reportError(static_cast<DataError>(DataError::kParameterReadError), m_parameter_index);
            }
            
            ++m_parameter_index;

            read(args...);
        }
        
        template <typename T, typename... Ts>
        void read_and_validate(T&& value, bool(*validator)(typename remove_reference<T>::type), Ts&&... args)
        {
            if (!m_in_command)
            {
                return;
            }

            read(value);

            if (!validator(value))
            {
                reportError(static_cast<DataError>(DataError::kParameterInvalid), m_parameter_index - 1u);
            }

            read_and_validate(args...);
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

        void reportError(const DataError code, const int16_t data = 0)
        {
            if (!m_in_command)
            {
                return;
            }

            m_writer.writeError(code);
            m_writer.write(data);
            
            longjmp(m_jmp_buf, code);
        }
    };
}

#endif // EMBMESSENGER_EMBMESSENGER_HPP
