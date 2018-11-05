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
    class EmbMessenger;

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
        Command();

        virtual void send(EmbMessenger* messenger);
        virtual void receive(EmbMessenger* messenger);
        virtual void reportError(const uint8_t error, const int16_t data, std::shared_ptr<Command> ptr);

        std::type_index getTypeIndex() const;
        uint16_t getMessageId() const;

        template <typename CommandType>
        void setCallback(std::function<void(std::shared_ptr<CommandType>)> callback)
        {
            m_callback = [=](std::shared_ptr<Command> ptr) {
                callback(std::static_pointer_cast<CommandType>(ptr));
            };
        }

#ifndef EMB_SINGLE_THREADED
        void wait();
#endif
    };
}  // namespace emb

#endif  // EMBMESSENGER_COMMAND_HPP