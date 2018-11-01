#ifndef ARDUINOBUFFER_HPP
#define ARDUINOBUFFER_HPP

#include <EmbMessenger/IBuffer.hpp>
#include <EmbMessenger/DataType.hpp>
#include <Stream.h>

template <uint8_t BufferSize>
class ArduinoBuffer : public emb::IBuffer
{
protected:
    Stream* m_stream;
    uint8_t m_buffer[BufferSize];
    uint8_t m_streamFront, m_readFront;
    uint8_t m_numberMessages;

public:
    ArduinoBuffer(Stream* stream)
    {
        m_stream = stream;
        m_streamFront = 0;
        m_readFront = 0;
        m_numberMessages = 0;
    }

    void writeByte(const uint8_t byte) override
    {
        m_stream->write(byte);
    }

    uint8_t peek() const override
    {
        return m_buffer[m_readFront];
    }

    uint8_t readByte() override
    {
        uint8_t byte = m_buffer[m_readFront];
        if (m_buffer[(BufferSize + m_readFront - 1) % BufferSize] == emb::DataType::kCrc)
        {
            --m_numberMessages;
        }
        m_readFront = (m_readFront + 1) % BufferSize;
        return byte;
    }

    bool empty() const override
    {
        return m_streamFront == m_readFront;
    }

    size_t size() const override
    {
        return (BufferSize + m_streamFront - m_readFront) % BufferSize;
    }

    uint8_t messagesAvailable() const override
    {
        return m_numberMessages;
    }

    void update() override
    {
        while (m_stream->available() > 0)
        {
            m_buffer[m_streamFront] = m_stream->read();
            if (m_buffer[(BufferSize + m_streamFront - 1) % BufferSize] == emb::DataType::kCrc)
            {
                ++m_numberMessages;
            }
            m_streamFront = (m_streamFront + 1) % BufferSize;
        }
    }

    void zero() override
    {
        m_streamFront = 0;
        m_readFront = 0;
        m_numberMessages = 0;
    }
};

#endif // ARDUINOBUFFER_HPP
