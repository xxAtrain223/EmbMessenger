#include "EmbMessenger/Command.hpp"

namespace emb
{
    namespace host
    {
        Command::Command() : m_type_index(typeid(Command))
        {
        }

        void Command::send(EmbMessenger* messenger)
        {
        }

        void Command::receive(EmbMessenger* messenger)
        {
        }

        void Command::reportError(const uint8_t error, const int16_t data, std::shared_ptr<Command> ptr)
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

#ifndef EMB_SINGLE_THREADED
        void Command::wait()
        {
            m_is_waiting = true;
            std::unique_lock<std::mutex> lk(m_mutex);
            m_condition_variable.wait(lk, [&] { return m_received; });
        }
#endif
    }  // namespace host
}  // namespace emb