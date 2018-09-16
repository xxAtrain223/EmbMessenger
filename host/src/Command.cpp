#include "EmbMessenger/Command.hpp"

namespace emb
{
    Command::Command() :
        m_type_index(typeid(Command))
    {
    }

    void Command::send(EmbMessenger * messenger)
    {
    }

    void Command::receive(EmbMessenger * messenger)
    {
    }

    void Command::reportError(const uint8_t error, std::shared_ptr<Command> ptr)
    {
    }

    std::type_index Command::getTypeIndex() const
    {
        return m_type_index;
    }

    uint16_t Command::getMessageId() const
    {
        return m_message_id;
    }
}