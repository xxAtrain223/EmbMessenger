#ifndef EMBMESSENGER_COMMAND_HPP
#define EMBMESSENGER_COMMAND_HPP

#include "EmbMessenger/EmbMessenger.hpp"

#include <cstdint>
#include <functional>
#include <memory>

namespace emb
{
    class Command
    {
    protected:
        friend class EmbMessenger;

        uint16_t m_message_id;
        std::function<void(std::shared_ptr<Command>)> m_callback = nullptr;

        virtual void send(EmbMessenger* messenger);
        virtual void receive(EmbMessenger* messenger);

    public:
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