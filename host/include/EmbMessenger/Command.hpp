#ifndef EMBMESSENGER_COMMAND_HPP
#define EMBMESSENGER_COMMAND_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <typeindex>

#ifndef EMB_SINGLE_THREADED
#include <condition_variable>
#include <mutex>
#endif

namespace emb
{
    namespace host
    {
        class EmbMessenger;

        /**
         * @brief Base class for any commands to send to the device.
         * 
         * Inherit from this class and register it using EmbMessenger::registerCommand.
         * In your constructor, be sure to set `m_type_index`. This is needed if you ever send it using the nontemplate version of `EmbMessenger::send`.
         * Override `send` to send values to the device.
         * Override `receive` to receive values from the device.
         * Override `reportError` if your command throws custom errors.
         */
        class Command
        {
            friend class EmbMessenger;

        protected:
            std::type_index m_type_index;
            uint16_t m_message_id;
            std::function<void(std::shared_ptr<Command>)> m_callback = nullptr;
            bool m_is_periodic = false;

#ifndef EMB_SINGLE_THREADED
            std::mutex m_mutex;
            std::condition_variable m_condition_variable;
            bool m_is_waiting = false;
            bool m_received = false;
#endif

        public:
            /**
             * @brief Construct a new Command.
             * 
             * Sets the type_index.
             */
            Command();

            /**
             * @brief Overload to send values to the device.
             * 
             * @param messenger The messenger to write values to the device
             */
            virtual void send(EmbMessenger* messenger);

            /**
             * @brief Overload to receive values from the device.
             * 
             * @param messenger The messenger to read values from the device
             */
            virtual void receive(EmbMessenger* messenger);

            /**
             * @brief Overload to catch errors from the device and throw as exceptions.
             * 
             * @param error Error code from user command on the device
             * @param data Data associated with the error
             * @param ptr Pointer to the command that threw the error
             */
            virtual void reportError(const uint8_t error, const int16_t data, std::shared_ptr<Command> ptr);

            /**
             * @brief Get the std::type_index of the command.
             * 
             * @return std::type_index of the command
             */
            std::type_index getTypeIndex() const;

            /**
             * @brief Get the Message Id for the command.
             * 
             * @return The command's message ID
             */
            uint16_t getMessageId() const;

            /**
             * @brief Set a callback to be called after the command has received a message from the device.
             * 
             * @tparam CommandType The type of your command
             * @param callback Your callback to be called
             */
            template <typename CommandType>
            void setCallback(std::function<void(std::shared_ptr<CommandType>)> callback)
            {
                m_callback = [=](std::shared_ptr<Command> ptr) {
                    callback(std::static_pointer_cast<CommandType>(ptr));
                };
            }

#ifndef EMB_SINGLE_THREADED
            /**
             * @brief Have your thread wait until the command receives a response from the device.
             */
            void wait();
#endif
        };
    }  // namespace host
}  // namespace emb

#endif  // EMBMESSENGER_COMMAND_HPP