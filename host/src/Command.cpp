#include "EmbMessenger/Command.hpp"

namespace emb
{
    void Command::send(EmbMessenger * messenger)
    {
    }

    void Command::receive(EmbMessenger * messenger)
    {
    }

    uint16_t Command::getMessageId()
    {
        return m_message_id;
    }
}