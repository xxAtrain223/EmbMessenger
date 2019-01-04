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
#endif

namespace emb
{
    namespace host
    {
        class EmbMessenger
        {
        protected:
            shared::IBuffer* m_buffer;
            shared::Writer m_writer;
            shared::Reader m_reader;

            uint16_t m_message_id;
            std::map<std::type_index, uint8_t> m_command_ids;
            std::map<uint16_t, std::shared_ptr<Command>> m_commands;
            std::shared_ptr<Command> m_current_command;
            uint8_t m_parameter_index;

#ifndef EMB_SINGLE_THREADED
            std::function<bool(std::exception_ptr)> m_exception_handler;

            std::thread m_update_thread;
            bool m_running;
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
            EmbMessenger(shared::IBuffer* buffer, std::chrono::milliseconds init_timeout = std::chrono::seconds(10));

            void update();
#else
            EmbMessenger(shared::IBuffer* buffer, std::function<bool(std::exception_ptr)> exception_handler,
                         std::chrono::milliseconds init_timeout = std::chrono::seconds(10));
            ~EmbMessenger();

            bool running() const;

            void stop();
#endif

            template <typename T>
            void registerCommand(const uint8_t id)
            {
                m_command_ids.emplace(typeid(T), id);
            }

            std::shared_ptr<Command> send(std::shared_ptr<Command> command);

            template <typename CommandType>
            std::shared_ptr<CommandType> send(std::shared_ptr<CommandType> command)
            {
                command->m_message_id = m_message_id;
                {
#ifndef EMB_SINGLE_THREADED
                    std::lock_guard<std::mutex> lock(m_commands_mutex);
#endif
                    m_commands.emplace(m_message_id, command);
                }

                write(m_message_id++, m_command_ids.at(typeid(CommandType)));
                command->send(this);
                m_writer.writeCrc();

                return command;
            }

            template <typename T, typename... Ts>
            void write(const T arg, Ts... args)
            {
                m_writer.write(arg);
                write(args...);
            }

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

        protected:
            class ResetCommand : public Command
            {
            public:
                ResetCommand();
            };

            class RegisterPeriodicCommand : public Command
            {
                uint8_t m_command_id;
                uint32_t m_period;

            public:
                RegisterPeriodicCommand(uint8_t commandId, uint32_t period);

                virtual void send(EmbMessenger* messenger);
            };

            class UnregisterPeriodicCommand : public Command
            {
                uint8_t m_command_id;

            public:
                uint16_t m_periodic_message_id;

                UnregisterPeriodicCommand(uint8_t commandId);

                virtual void send(EmbMessenger* messenger);
                virtual void receive(EmbMessenger* messenger);
            };

        public:
            void resetDevice();

            template <typename CommandType, typename CallbackType>
            std::shared_ptr<CommandType> registerPeriodicCommand(uint32_t period, CallbackType callback)
            {
                std::shared_ptr<CommandType> periodic_command = std::make_shared<CommandType>();
                periodic_command->m_is_periodic = true;
                // Template dumbness/magic https://stackoverflow.com/a/2105906/4776344
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