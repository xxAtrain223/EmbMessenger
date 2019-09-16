#ifndef EMBMESSENGER_EMBMESSENGER_HPP
#define EMBMESSENGER_EMBMESSENGER_HPP

#include <setjmp.h>
#include <stdint.h>

#include "EmbMessenger/DataError.hpp"
#include "EmbMessenger/Reader.hpp"
#include "EmbMessenger/Writer.hpp"
#include "Templates.hpp"

#ifdef EMB_TESTING
#include <functional>
#endif

namespace emb
{
    namespace shared
    {
        class IBuffer;
    }

    namespace device
    {
        /**
         * @brief The messenger class for the device.
         * 
         * EmbMessenger has reserved 16 command IDs for internal usage
         * 
         * @tparam MaxCommands The maximum number of commands to store, must be less than `65520`
         * @tparam MaxPeriodicCommands The maximum number of periodic commands to store, must be less than `65520`
         */
        template <uint8_t MaxCommands, uint8_t MaxPeriodicCommands>
        class EmbMessenger
        {
        protected:
#ifndef EMB_TESTING
            using CommandFunction = void (*)(void);
            using TimeFunction = uint32_t (*)(void);
#else
            using CommandFunction = std::function<void()>;
            using TimeFunction = std::function<uint32_t()>;
#endif

            struct PeriodicCommand
            {
                uint16_t command_id = 65535;
                uint16_t message_id = 0;
                uint32_t time_interval = 0;
                uint32_t next_time = 0;
            };

            shared::IBuffer* m_buffer;
            shared::Reader m_reader;
            shared::Writer m_writer;
            jmp_buf m_jmp_buf;

            uint16_t m_command_id;
            uint16_t m_message_id;
            bool m_is_periodic;
            uint8_t m_num_messages;

            uint16_t m_parameter_index;

            TimeFunction m_time_func;

            CommandFunction m_commands[MaxCommands];
            PeriodicCommand m_periodic_commands[MaxPeriodicCommands];

            template <typename T, typename... Ts>
            void read_helper(false_type, T& value, Ts&&... args)
            {
                read(value);
                read(args...);
            }

            template <typename T, typename F, typename... Ts>
            void read_helper(true_type, T& value, F&& validate)
            {
                read(value);

                if (!validate(value))
                {
                    reportError(shared::DataError::kParameterInvalid, m_parameter_index - 1u);
                }

                checkCrc();
            }

            template <typename T, typename F, typename... Ts>
            void read_helper(true_type, T& value, F&& validate, Ts&&... args)
            {
                read(value);

                if (!validate(value))
                {
                    reportError(shared::DataError::kParameterInvalid, m_parameter_index - 1u);
                }

                read(args...);
            }

            void write()
            {
                (void)0;
            }

            void consumeMessage()
            {
                for (uint8_t n = m_buffer->messages();
                     n > 0 && n == m_buffer->messages() && n == m_num_messages && !m_buffer->empty();
                     m_buffer->readByte())
                    ;
            }

            void resetPeriodicCommands()
            {
                checkCrc();
                for (uint8_t i = 0; i < MaxPeriodicCommands; ++i)
                {
                    m_periodic_commands[i].command_id = 65535;
                    m_periodic_commands[i].message_id = 0;
                    m_periodic_commands[i].time_interval = 0;
                    m_periodic_commands[i].next_time = 0;
                }
            }

            void registerPeriodicCommand()
            {
                uint8_t periodic_command_id;
                uint32_t periodic_interval;

                read(periodic_command_id, periodic_interval);

                if (m_commands[periodic_command_id] == nullptr)
                {
                    reportError(shared::DataError::kParameterInvalid, 0);
                }

                bool registeredCommand = false;
                for (uint8_t i = 0; i < MaxPeriodicCommands; ++i)
                {
                    if (m_periodic_commands[i].command_id >= 0xFFF0)
                    {
                        m_periodic_commands[i].command_id = periodic_command_id;
                        m_periodic_commands[i].message_id = m_message_id;
                        m_periodic_commands[i].time_interval = periodic_interval;
                        m_periodic_commands[i].next_time = m_time_func();
                        registeredCommand = true;
                        break;
                    }
                }

                if (!registeredCommand)
                {
                    reportError(shared::DataError::kOutOfPeriodicCommandSlots);
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

                        m_periodic_commands[i].command_id = 65535;
                        m_periodic_commands[i].message_id = 0;
                        m_periodic_commands[i].time_interval = 0;
                        m_periodic_commands[i].next_time = 0;
                        return;
                    }
                }

                reportError(shared::DataError::kParameterInvalid, 0);
            }

        public:
            /**
             * @brief Constructor for EmbMessenger without periodic commands.
             * 
             * To use periodic commands, you must pass in a time function. e.g. `millis()`
             * 
             * @param buffer Buffer for communication
             */
            EmbMessenger(shared::IBuffer* buffer) :
                m_buffer(buffer),
                m_reader(buffer),
                m_writer(buffer)
            {
                static_assert(MaxCommands < 0xFFF0, "MaxCommands must be less than 0xFFF0 (65520)");
                static_assert(MaxPeriodicCommands == 0, "TimeFunction required for periodic commands");

                for (uint8_t i = 0; i < MaxCommands; ++i)
                {
                    m_commands[i] = nullptr;
                }
            }

            /**
             * @brief Constructor for EmbMessenger with periodic commands.
             * 
             * @param buffer Buffer for communication
             * @param timeFunc A function that keeps track of the time. e.g. `millis()`
             */
            EmbMessenger(shared::IBuffer* buffer, TimeFunction timeFunc) :
                m_buffer(buffer),
                m_reader(buffer),
                m_writer(buffer),
                m_time_func(timeFunc)
            {
                static_assert(MaxCommands < 0xFFF0, "MaxCommands must be less than 0xFFF0 (65520)");
                static_assert(MaxPeriodicCommands < 0xFFF0, "MaxPeriodicCommands must be less than 0xFFF0 (65520)");

                for (uint8_t i = 0; i < MaxCommands; ++i)
                {
                    m_commands[i] = nullptr;
                }
            }

            /**
             * @brief Registers a command
             * 
             * Command must be less than `0xFFF0` (`65520`).
             * Command must be less than MaxCommands.
             * Command IDs cannot be reused/overridden.
             * 
             * @param id Unique ID for the command
             * @param command The command to be registered
             * @return True if successful
             */
            bool registerCommand(uint8_t id, CommandFunction command)
            {
                if (id >= 0xFFF0)
                {
                    return false;
                }

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

            /**
             * @brief Get the Command ID
             * 
             * For debugging, to be used within your command for sanity checks.
             * Calling this from outside a command is undefined behaviour.
             * 
             * @return uint8_t Current command ID
             */
            uint8_t getCommandId() const
            {
                return m_command_id;
            }

            /**
             * @brief Get the Message ID
             * 
             * For debugging, to be used within your command for sanity checks.
             * Calling this from outside a command is undefined behaviour.
             * 
             * @return uint16_t Current Message ID
             */
            uint16_t getMessageId() const
            {
                return m_message_id;
            }

            /**
             * @brief Determine if your command is being executed as a periodic command.
             * 
             * Calling this from outside a command is undefined behaviour.
             * 
             * @return true Your command is being executed as a periodic command.
             * @return false Your command is being executed as a standard command from the host.
             */
            bool getIsPeriodic() const
            {
                return m_is_periodic;
            }

            /**
             * @brief Update method for EmbMessenger.
             * 
             * This should be called as frequently as possible. e.g. in `loop()`
             * Updates the buffer, reads data from the buffer for running commands, executes commands, and runs periodic commands.
             */
            void update()
            {
                m_buffer->update();
                m_num_messages = m_buffer->messages();

                if (m_num_messages != 0)
                {
                    m_reader.resetCrc();
                    m_message_id = 0;
                    m_parameter_index = 0;

                    if (!m_reader.read(m_message_id))
                    {
                        m_writer.writeError(shared::DataError::kMessageIdReadError);
                        m_writer.write((uint8_t)0);
                        consumeMessage();
                        m_writer.writeCrc();
                        return;
                    }
                    m_writer.write(m_message_id);

                    if (!m_reader.read(m_command_id))
                    {
                        m_writer.writeError(shared::DataError::kCommandIdReadError);
                        m_writer.write((uint8_t)0);
                        consumeMessage();
                        m_writer.writeCrc();
                        return;
                    }

                    if (setjmp(m_jmp_buf) == 0)
                    {
                        switch (m_command_id)
                        {
                            case 0xFFFF:
                                resetPeriodicCommands();
                                break;
                            case 0xFFFE:
                                registerPeriodicCommand();
                                break;
                            case 0xFFFD:
                                unregisterPeriodicCommand();
                                break;
                            default:
                                if (m_command_id >= MaxCommands || m_commands[m_command_id] == nullptr)
                                {
                                    m_writer.writeError(shared::DataError::kCommandIdInvalid);
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

                    if (m_buffer->messages() == m_num_messages)
                    {
                        if (m_reader.nextCrc())
                        {
                            if (!m_reader.readCrc())
                            {
                                consumeMessage();
                                m_writer.writeError(shared::DataError::kCrcInvalid);
                                m_writer.write((uint8_t)0);
                            }
                        }
                        else
                        {
                            consumeMessage();
                            m_writer.writeError(shared::DataError::kExtraParameters);
                            m_writer.write((uint8_t)0);
                        }
                    }

                    m_writer.writeCrc();
                }

                if (MaxPeriodicCommands > 0)
                {
                    m_is_periodic = true;
                    uint32_t current_time = m_time_func();
                    for (uint8_t i = 0; i < MaxPeriodicCommands; ++i)
                    {
                        if (m_periodic_commands[i].command_id < 0xFFF0 && m_periodic_commands[i].next_time <= current_time)
                        {
                            m_command_id = m_periodic_commands[i].command_id;
                            m_message_id = m_periodic_commands[i].message_id;
                            m_parameter_index = 0;

                            m_writer.write(m_message_id);
                            if (setjmp(m_jmp_buf) == 0)
                            {
                                m_commands[m_command_id]();
                            }
                            m_writer.writeCrc();
                            m_periodic_commands[i].next_time += m_periodic_commands[i].time_interval;
                            current_time = m_time_func();
                        }
                    }
                }
            }

            /**
             * @brief Checks the CRC.
             * 
             * Calling this is not neccessary if you read any values because it will be checked automatically.
             * Call this if you don't read any values and you need to ensure the correct command is being executed.
             * Calling this from outside a command is undefined behaviour.
             */
            void checkCrc()
            {
                if (m_reader.nextCrc() && !m_reader.readCrc())
                {
                    reportError(shared::DataError::kCrcInvalid);
                }
            }

            /**
             * @brief Use in commands to read a value from the host.
             * 
             * Calling this from outside a command is undefined behaviour.
             * 
             * @tparam T Type of the value to read
             * @param value Variable to store the value
             */
            template <typename T>
            void read(T& value)
            {
                if (!m_reader.read(value))
                {
                    reportError(shared::DataError::kParameterReadError, m_parameter_index);
                }

                ++m_parameter_index;

                checkCrc();
            }

            /**
             * @brief Use in commands to read multiple values and/or to validate values from the host.
             * 
             * Passing a function/lambda after a variable will use the function as a validator.
             * A validator should take a single parameter with the same type as the variable before it in the read call.
             * e.g.
             * ```
             * int16_t a = 0;
             * messenger.read(a, [](int16_t val) { return val != 0; });
             * ```
             * 
             * Calling this from outside a command is undefined behaviour.
             * 
             * @tparam T Type of the first parameter
             * @tparam F Type of the second parameter
             * @tparam Ts Types of the rest of the parameters
             * @param value Variable to store the value
             * @param validator A validator for the first parameter or a second variable to store a value
             * @param args The rest of the parameters
             */
            template <typename T, typename F, typename... Ts>
            void read(T& value, F&& validator, Ts&&... args)
            {
                read_helper(is_validator_t<T, F>{}, value, validator, args...);
            }

            /**
             * @brief Use in commands to write values to the host.
             * 
             * Calling this from outside a command is undefined behaviour.
             * 
             * @tparam T Type of the first parameter
             * @tparam Ts Types of the rest of the parameters
             * @param value First parameter
             * @param args The rest of the parameters
             */
            template <typename T, typename... Ts>
            void write(const T value, const Ts... args)
            {
                m_writer.write(value);

                write(args...);
            }

            /**
             * @brief Use this to interrupt execution and send an error to the host.
             * 
             * Calling this will `longjmp` and exit your command immediately.
             * 
             * emb::shared::DataErrors have predefined exceptions on the host ready to convert the error.
             * 
             * You can pass custom errors to be handled by your overridden Command::reportError in your Command.
             * Custom errors should start at `0x20` as `0x00` - `0x1F` are reserved for EmbMessenger.
             * 
             * The data parameter can be used for further information about your error.
             * 
             * Calling this from outside a command is undefined behaviour.
             * 
             * @param code Error code to be sent to the host
             * @param data Data to be sent to the host with the error code
             */
            void reportError(const uint8_t code, const int16_t data = 0)
            {
                m_writer.writeError(static_cast<shared::DataError>(code));
                m_writer.write(data);

                longjmp(m_jmp_buf, code);
            }
        };
    }  // namespace device
}  // namespace emb

#endif  // EMBMESSENGER_EMBMESSENGER_HPP
