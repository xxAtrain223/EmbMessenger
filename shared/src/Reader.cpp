#include "EmbMessenger/Reader.hpp"

#include "EmbMessenger/Crc.hpp"
#include "EmbMessenger/DataType.hpp"
#include "EmbMessenger/IBuffer.hpp"

namespace emb
{
    bool Reader::peekByte(uint8_t& value) const
    {
        if (m_buffer->empty())
        {
            return false;
        }

        value = m_buffer->peek();
        return true;
    }

    bool Reader::readByte(uint8_t& value)
    {
        if (m_buffer->empty())
        {
            return false;
        }

        value = m_buffer->readByte();
        m_crc = crc::Calculate8(m_crc, value);
        return true;
    }

    bool Reader::removeByte()
    {
        uint8_t value;
        return readByte(value);
    }

    Reader::Reader(IBuffer* buffer)
    {
        m_buffer = buffer;
        m_crc = 0;
    }

    bool Reader::getType(DataType& type) const
    {
        uint8_t byte;

        if (!peekByte(byte))
        {
            return false;
        }

        if (byte <= 0x7F)
        {
            type = DataType::kPosFixInt;
            return true;
        }
        else if (byte >= 0xE0)
        {
            type = DataType::kNegFixInt;
            return true;
        }

        switch (byte)
        {
        case DataType::kNull:
            type = DataType::kNull;
            break;
        case DataType::kBoolFalse:
            type = DataType::kBoolFalse;
            break;
        case DataType::kBoolTrue:
            type = DataType::kBoolTrue;
            break;
        case DataType::kFloat:
            type = DataType::kFloat;
            break;
        case DataType::kUint8:
            type = DataType::kUint8;
            break;
        case DataType::kUint16:
            type = DataType::kUint16;
            break;
        case DataType::kUint32:
            type = DataType::kUint32;
            break;
        case DataType::kUint64:
            type = DataType::kUint64;
            break;
        case DataType::kInt8:
            type = DataType::kInt8;
            break;
        case DataType::kInt16:
            type = DataType::kInt16;
            break;
        case DataType::kInt32:
            type = DataType::kInt32;
            break;
        case DataType::kInt64:
            type = DataType::kInt64;
            break;
        case DataType::kCrc:
            type = DataType::kCrc;
            break;
        case DataType::kError:
            type = DataType::kError;
            break;
        default:
            return false;
        }
        
        return true;
    }

    bool Reader::nextBool() const
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kBoolFalse:
        case DataType::kBoolTrue:
            return true;
        }

        return false;
    }

    bool Reader::nextChar() const
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        return type == DataType::kPosFixInt;
    }

    bool Reader::nextSignedInt() const
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kInt8:
        case DataType::kInt16:
        case DataType::kInt32:
        case DataType::kInt64:
        case DataType::kPosFixInt:
        case DataType::kNegFixInt:
            return true;
        }

        return false;
    }

    bool Reader::nextUnsignedInt() const
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kUint8:
        case DataType::kUint16:
        case DataType::kUint32:
        case DataType::kUint64:
        case DataType::kPosFixInt:
            return true;
        }

        return false;
    }
    
    bool Reader::nextFloat() const
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        return type == DataType::kFloat;
    }

    bool Reader::nextNull() const
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        return type == DataType::kNull;
    }

    bool Reader::nextError() const
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        return type == DataType::kError;
    }

    bool Reader::nextCrc() const
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        return type == DataType::kCrc;
    }

    bool Reader::read(bool& value)
    {
        if (!nextBool())
        {
            return false;
        }

        uint8_t byte;
        readByte(byte);

        if (byte == 0xC2)
        {
            value = false;
        }
        else if (byte == 0xC3)
        {
            value = true;
        }

        return true;
    }

    bool Reader::read(uint8_t& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kPosFixInt:
            readByte(value);
            return true;
        case DataType::kUint8:
            return removeTypeAndRead(value);
        default:
            return false;
        }
    }

    bool Reader::read(uint16_t& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kPosFixInt:
        case DataType::kUint8:
            return readAndConvert<uint8_t>(value);
        case DataType::kUint16:
            return removeTypeAndRead(value);
        default:
            return false;
        }
    }

    bool Reader::read(uint32_t& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kPosFixInt:
        case DataType::kUint8:
        case DataType::kUint16:
            return readAndConvert<uint16_t>(value);
        case DataType::kUint32:
            return removeTypeAndRead(value);
        default:
            return false;
        }
    }

    bool Reader::read(uint64_t& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kPosFixInt:
        case DataType::kUint8:
        case DataType::kUint16:
        case DataType::kUint32:
            return readAndConvert<uint32_t>(value);
        case DataType::kUint64:
            return removeTypeAndRead(value);
        default:
            return false;
        }
    }

    bool Reader::read(int8_t& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        uint8_t data;
        switch (type)
        {
        case DataType::kPosFixInt:
            readByte(data);
            value = data;
            return true;
        case DataType::kNegFixInt:
            readByte(data);
            value = data;
            return true;
        case DataType::kInt8:
            return removeTypeAndRead(value);
        default:
            return false;
        }
    }

    bool Reader::read(int16_t& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kPosFixInt:
        case DataType::kNegFixInt:
        case DataType::kInt8:
            return readAndConvert<int8_t>(value);
        case DataType::kInt16:
            return removeTypeAndRead(value);
        default:
            return false;
        }
    }

    bool Reader::read(int32_t& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kPosFixInt:
        case DataType::kNegFixInt:
        case DataType::kInt8:
        case DataType::kInt16:
            return readAndConvert<int16_t>(value);
        case DataType::kInt32:
            return removeTypeAndRead(value);
        default:
            return false;
        }
    }

    bool Reader::read(int64_t& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        switch (type)
        {
        case DataType::kPosFixInt:
        case DataType::kNegFixInt:
        case DataType::kInt8:
        case DataType::kInt16:
        case DataType::kInt32:
            return readAndConvert<int32_t>(value);
        case DataType::kInt64:
            return removeTypeAndRead(value);
        default:
            return false;
        }
    }

    bool Reader::read(float& value)
    {
        DataType type;
        if (!getType(type))
        {
            return false;
        }

        if (type == DataType::kFloat && m_buffer->size() >= sizeof(value) + 1)
        {
            removeByte(); // Remove Type Byte
            return readData(value);
        }

        return false;
    }

    bool Reader::readNull()
    {
        if (!nextNull())
        {
            return false;
        }

        removeByte(); // Remove Type Byte

        return true;
    }

    bool Reader::readError(DataError& value)
    {
        if (!nextError())
        {
            return false;
        }

        if (m_buffer->size() >= sizeof(value) + 1)
        {
            removeByte();
            return readData(value);
        }

        return false;
    }

    bool Reader::readCrc()
    {
        if (!nextCrc())
        {
            return false;
        }

        if (m_buffer->size() >= 2)
        {
            removeByte(); // Remove Type Byte
            removeByte(); // Consume the CRC byte, Updates internal CRC

            return m_crc == 0x00;
        }

        return false;
    }

    void Reader::resetCrc()
    {
        m_crc = 0;
    }
}