#include "EmbMessenger/EmbMessenger.hpp"

namespace emb
{
    EmbMessenger::EmbMessenger(IBuffer* buffer) :
        m_buffer(buffer),
        m_writer(buffer),
        m_reader(buffer)
    {
        m_message_id = 0;
    }

    void EmbMessenger::write()
    {
        (void)0; // Noop
    }

    void EmbMessenger::read()
    {
        while (m_reader.nextError())
        {
            uint8_t error = 0;
            m_reader.readError(error);
            // TODO: Throw exception
            printf("Error: 0x%02X\n", error);
        }
    }
}