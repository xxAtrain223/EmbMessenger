#include "EmbMessenger/Writer.hpp"

#include "EmbMessenger/Crc.hpp"
#include "EmbMessenger/DataType.hpp"
#include "EmbMessenger/IBuffer.hpp"

namespace emb
{
    namespace shared
    {
        void Writer::writeByte(const uint8_t byte)
        {
            m_buffer->writeByte(byte);
            m_crc = crc::Calculate8(m_crc, byte);
        }

        Writer::Writer(IBuffer* buffer)
        {
            m_buffer = buffer;
            m_crc = 0;
        }

        void Writer::write(const bool value)
        {
            if (value)
            {
                writeByte(DataType::kBoolTrue);
            }
            else
            {
                writeByte(DataType::kBoolFalse);
            }
        }

        void Writer::write(const uint8_t value)
        {
            if (value <= 0x7F)
            {
                // PosFixInt
                writeByte(value);
            }
            else
            {
                writeByte(DataType::kUint8);
                writeData(value);
            }
        }

        void Writer::write(const uint16_t value)
        {
            uint8_t value8 = static_cast<uint8_t>(value);

            if (value == value8)
            {
                // Value fits into uint8_t
                write(value8);
            }
            else
            {
                writeByte(DataType::kUint16);
                writeData(value);
            }
        }

        void Writer::write(const uint32_t value)
        {
            uint16_t value16 = static_cast<uint16_t>(value);

            if (value == value16)
            {
                // Value fits into uint16_t
                write(value16);
            }
            else
            {
                writeByte(DataType::kUint32);
                writeData(value);
            }
        }

        void Writer::write(const uint64_t value)
        {
            uint32_t value32 = static_cast<uint32_t>(value);

            if (value == value32)
            {
                // Value fits into uint32_t
                write(value32);
            }
            else
            {
                writeByte(DataType::kUint64);
                writeData(value);
            }
        }

        void Writer::write(const int8_t value)
        {
            if (value <= (int8_t)0x7F && value >= 0)
            {
                // PosFixInt
                writeByte(value);
            }
            else if (value >= (int8_t)0xE0)
            {
                // NegFixInt
                writeByte(value);
            }
            else
            {
                writeByte(DataType::kInt8);
                writeData(value);
            }
        }

        void Writer::write(const int16_t value)
        {
            int8_t value8 = static_cast<int8_t>(value);

            if (value == value8)
            {
                // Value fits into int8_t
                write(value8);
            }
            else
            {
                writeByte(DataType::kInt16);
                writeData(value);
            }
        }

        void Writer::write(const int32_t value)
        {
            int16_t value16 = static_cast<int16_t>(value);

            if (value == value16)
            {
                // Value fits into int16_t
                write(value16);
            }
            else
            {
                writeByte(DataType::kInt32);
                writeData(value);
            }
        }

        void Writer::write(const int64_t value)
        {
            int32_t value32 = static_cast<int32_t>(value);

            if (value == value32)
            {
                // Value fits into int32_t
                write(value32);
            }
            else
            {
                writeByte(DataType::kInt64);
                writeData(value);
            }
        }

        void Writer::write(const float value)
        {
            writeByte(DataType::kFloat);
            writeData(value);
        }

        void Writer::writeNull()
        {
            writeByte(DataType::kNull);
        }

        void Writer::writeError(const DataError value)
        {
            writeByte(DataType::kError);
            writeByte(value);
        }

        void Writer::writeCrc()
        {
            writeByte(DataType::kEndOfMessage);
            writeData(m_crc);
        }
    }  // namespace shared
}  // namespace emb
