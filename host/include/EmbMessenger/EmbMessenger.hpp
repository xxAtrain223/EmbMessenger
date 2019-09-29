#ifndef EMBMESSENGER_EMBMESSENGER_HPP
#define EMBMESSENGER_EMBMESSENGER_HPP

#include "EmbMessenger/Command.hpp"
#include "EmbMessenger/Exceptions.hpp"
#include "EmbMessenger/IBuffer.hpp"
#include "EmbMessenger/Reader.hpp"
#include "EmbMessenger/Writer.hpp"

#include <chrono>
#include <map>
#include <memory>
#include <typeindex>

#ifndef EMB_SINGLE_THREADED
#include <mutex>
#include <thread>
#include <atomic>
#endif

namespace emb
{
    namespace host
    {
        /**
         * @brief The messenger class for host computer. 
         * 
         * This class is responsible for the communication between the host and device.
         */
        class EmbMessenger
        {
        protected:
            shared::IBuffer* m_buffer;
            shared::Writer m_writer;
            shared::Reader m_reader;

            uint16_t m_message_id;
            std::map<std::type_index, uint16_t> m_command_ids;
            std::map<uint16_t, std::shared_ptr<Command>> m_commands;
            std::shared_ptr<Command> m_current_command;
            uint8_t m_parameter_index;

#ifndef EMB_SINGLE_THREADED
            std::function<bool(std::exception_ptr)> m_exception_handler;

            std::thread m_update_thread;
            std::atomic_bool m_running;
            std::mutex m_commands_mutex;

            void update();
            void updateThread();
#endif

            void write();
            void read();

            void readErrors();
            void consumeMessage();

        public:
#ifdef EMB_SINGLE_THREADED
            /**
             * @brief Constructor for the Single Threaded EmbMessenger.
             * 
             * @param buffer Buffer for communication
             * @param init_timeout Time to keep retrying to establish a connection
             */
            EmbMessenger(shared::IBuffer* buffer, std::chrono::milliseconds init_timeout = std::chrono::seconds(10));

            /**
             * @brief Public update method for the Single Threaded EmbMessenger.
             * 
             * This should be called whenever a message has been received from the device or faster.
             */
            void update();
#else
            /**
             * @brief Constructor for the Multi Threaded EmbMessenger.
             * 
             * @param buffer Buffer for communication
             * @param exception_handler Handler for exceptions thrown in the update thread
             * @param init_timeout Time to keep retrying to establish a connection
             */
            EmbMessenger(shared::IBuffer* buffer, std::function<bool(std::exception_ptr)> exception_handler,
                         std::chrono::milliseconds init_timeout = std::chrono::seconds(10));

            /**
             * @brief Destructor for the Multi Threaded EmbMessenger.
             * 
             * Stops the update thread and joins it.
             */
            ~EmbMessenger();

            /**
             * @brief Get the status of the update thread.
             * 
             * @return Returns `true` if the update thread is running
             */
            bool running() const;

            /**
             * @brief Tell the update thread to stop running.
             * 
             * Does not block, the update thread will stop after finishing its current loop.
             */
            void stop();
#endif

            /**
             * @brief Register a command.
             * 
             * @tparam CommandType The Command type
             * @param id The ID for the command type
             */
            template <typename CommandType>
            void registerCommand(const uint16_t id)
            {
                static_assert(!std::is_same<Command, CommandType>::value, "You can't register the base class Command.");
                static_assert(std::is_base_of<Command, CommandType>::value, "Ensure CommandType is derived from Command.");
                m_command_ids.emplace(typeid(CommandType), id);
            }

            /**
             * @brief Send a command to the device.
             * 
             * @param command Command to send
             * @return std::shared_ptr<Command> Command sent to the device
             */
            std::shared_ptr<Command> send(std::shared_ptr<Command> command);

            /**
             * @brief Send a command to the device.
             * 
             * @param command Command to send
             * @return std::shared_ptr<CommandType> Command sent to the device
             */
            template <typename CommandType>
            std::shared_ptr<CommandType> send(std::shared_ptr<CommandType> command)
            {
                command->m_type_index = typeid(CommandType);
                return std::static_pointer_cast<CommandType>(send(std::static_pointer_cast<Command>(command)));
            }

            /**
             * @brief Send a command to the device.
             *
             * @param command Command to send
             * @param commandId Id of the Command to send
             * @return std::shared_ptr<Command> Command sent to the device
             */
            std::shared_ptr<Command> send(std::shared_ptr<Command> command, uint16_t commandId);

            /**
             * @brief Send a command to the device.
             *
             * @param command Command to send
             * @param commandId Id of the Command to send
             * @return std::shared_ptr<CommandType> Command sent to the device
             */
            template <typename CommandType>
            std::shared_ptr<CommandType> send(std::shared_ptr<CommandType> command, uint16_t commandId)
            {
                return std::static_pointer_cast<CommandType>(send(std::static_pointer_cast<Command>(command), commandId));
            }

            /**
             * @brief Use in Command::send to write values to the device.
             * 
             * @tparam T Type of the first parameter
             * @tparam Ts Types of the rest of the parameters
             * @param arg First parameter
             * @param args The rest of the parameters
             */
            template <typename T, typename... Ts>
            void write(const T arg, Ts... args)
            {
                m_writer.write(arg);
                write(args...);
            }

            /**
             * @brief Use in Command::receive to read values from the device.
             * 
             * @tparam T Type of the first parameter
             * @tparam Ts Types of the rest of the parameters
             * @param arg First parameter
             * @param args The rest of the parameters
             */
            template <typename T, typename... Ts>
            void read(T& arg, Ts&... args)
            {
                readErrors();
                if (!m_reader.read(arg))
                {
                    throw ParameterReadError(ExceptionSource::Host, m_parameter_index, m_current_command);
                }
                ++m_parameter_index;
                read(args...);
            }

            /**
             * @brief Gets the state of all sent commands.
             *
             * @return True if the all commands sent have been received
             */
            bool commandsReceived();

        protected:
            class ResetCommand : public Command
            {
            public:
                ResetCommand();
            };

            class RegisterPeriodicCommand : public Command
            {
                uint16_t m_command_id;
                uint32_t m_period;

            public:
                RegisterPeriodicCommand(uint16_t commandId, uint32_t period);

                virtual void send(EmbMessenger* messenger);
            };

            class UnregisterPeriodicCommand : public Command
            {
                uint16_t m_command_id;

            public:
                uint16_t m_periodic_message_id;

                UnregisterPeriodicCommand(uint16_t commandId);

                virtual void send(EmbMessenger* messenger);
                virtual void receive(EmbMessenger* messenger);
            };

        public:
            /**
             * @brief Send the Reset command to the device.
             * 
             * Resets all periodic commands on the device.
             */
            void resetDevice();

            /**
             * @brief Register a Command to be executed on the device periodically.
             * 
             * @tparam CommandType The type of the command to send to the device
             * @tparam CallbackType The type of the callback, this should be `std::function<void(std::shared_ptr<CommandType>)>`
             * @param period The time to wait between each command execution. The unit is the same as the return value of the time function passed into the device messenger.
             * @param callback The callback to be called when the periodic command returns a message.
             * @return std::shared_ptr<CommandType> The command that will be receiving the messages.
             */
            template <typename CommandType, typename CallbackType>
            std::shared_ptr<CommandType> registerPeriodicCommand(uint32_t period, CallbackType callback)
            {
                std::shared_ptr<CommandType> periodic_command = std::make_shared<CommandType>();
                periodic_command->m_is_periodic = true;
                periodic_command->template setCallback<CommandType>(callback);
                periodic_command->m_message_id = m_message_id;

                std::shared_ptr<RegisterPeriodicCommand> registerCommand =
                    std::make_shared<RegisterPeriodicCommand>(m_command_ids.at(typeid(CommandType)), period);
                registerCommand->setCallback<RegisterPeriodicCommand>([=](auto&& registerCommand) {
#ifndef EMB_SINGLE_THREADED
                    std::lock_guard<std::mutex> lock(m_commands_mutex);
#endif
                    m_commands[periodic_command->getMessageId()] = periodic_command;
                });
                send(registerCommand);

#ifndef EMB_SINGLE_THREADED
                registerCommand->wait();
#endif

                return periodic_command;
            }

            /**
             * @brief Unregister a command that is being executed periodically on the device.
             * 
             * @tparam CommandType The type of the command that is being executed periodically
             */
            template <typename CommandType>
            void unregisterPeriodicCommand()
            {
                std::shared_ptr<UnregisterPeriodicCommand> unregisterCommand =
                    std::make_shared<UnregisterPeriodicCommand>(m_command_ids.at(typeid(CommandType)));
                unregisterCommand->setCallback<UnregisterPeriodicCommand>([=](auto&& unregisterCommand) {
#ifndef EMB_SINGLE_THREADED
                    std::lock_guard<std::mutex> lock(m_commands_mutex);
#endif
                    m_commands.erase(unregisterCommand->m_periodic_message_id);
                });
                send(unregisterCommand);

#ifndef EMB_SINGLE_THREADED
                unregisterCommand->wait();
#endif
            }
        };
    }  // namespace host
}  // namespace emb

#endif  // EMBMESSENGER_EMBMESSENGER_HPP
