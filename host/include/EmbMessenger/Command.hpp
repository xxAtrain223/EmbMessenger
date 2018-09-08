#ifndef EMBMESSENGER_COMMAND_HPP
#define EMBMESSENGER_COMMAND_HPP

#include <cstdint>
#include <functional>
#include <memory>

namespace emb
{
    class EmbMessenger;

    class Command
    {
        friend class EmbMessenger;

    protected:
        uint16_t m_message_id;
        std::function<void(std::shared_ptr<Command>)> m_callback = nullptr;
        bool m_is_periodic = false;

    public:
        virtual void send(EmbMessenger* messenger);
        virtual void receive(EmbMessenger* messenger);

        uint16_t getMessageId();

        template <typename CommandType>
        void setCallback(std::function<void(std::shared_ptr<CommandType>)> callback)
        {
            m_callback = [=](std::shared_ptr<Command> ptr) {
                callback(std::static_pointer_cast<CommandType>(ptr));
            };
        }
    };
}

#endif // EMBMESSENGER_COMMAND_HPP