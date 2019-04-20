#include "EmbMessenger/DebugBuffer.hpp"
#include "EmbMessenger/DataType.hpp"

#include <cstdio>
#include <sstream>
#include <iomanip>

namespace emb
{
    namespace host
    {
        // Helper for getting the number of used bytes
        constexpr const uint8_t getWidth(uint16_t value16)
        {
            return (value16 == static_cast<uint8_t>(value16)) ? 2 : 4;
        }

        // Helper for getting the number of used bytes
        constexpr const uint8_t getWidth(uint32_t value32)
        {
            return (value32 == static_cast<uint16_t>(value32)) ? getWidth(static_cast<uint16_t>(value32)) : 8;
        }

        // Helper for getting the number of used bytes
        constexpr const uint8_t getWidth(uint64_t value64)
        {
            return (value64 == static_cast<uint32_t>(value64)) ? getWidth(static_cast<uint32_t>(value64)) : 16;
        }

        DebugBuffer::DecodeBuffer::DecodeBuffer(BufferType type, std::function<void(std::string)> print_func) :
            m_type(type),
            m_print_func(print_func)
        {
        }

        // Primary function decoding a message
        void DebugBuffer::DecodeBuffer::decode()
        {
            using namespace emb::shared;
            Reader reader(this);
            uint16_t message_id;
            uint16_t param = 0;
            std::stringstream ss;

            // All messages have a message id
            if (!reader.read(message_id))
            {
                m_print_func("Error reading message id");
            }
            ss << (m_type == BufferType::Write ? "Write" : "Read ") << " Message Id: " << message_id;
            m_print_func(ss.str());
            ss.str("");

            // Messages from the host have command ids
            if (m_type == BufferType::Write)
            {
                uint16_t command_id = 0;
                if (!reader.read(command_id))
                {
                    m_print_func("Write Error reading command id");
                }
                ss << "Write Command Id: " << command_id;
                m_print_func(ss.str());
                ss.str("");
            }

            emb::shared::DataType type;
            while (reader.getType(type))
            {
                // Print the message's source
                ss.str("");
                if (m_type == BufferType::Read)
                {
                    ss << "Read  ";
                }
                else if (m_type == BufferType::Write)
                {
                    ss << "Write ";
                }

                // Print the parameter index
                ss << std::setw(3) << param << ": ";

                switch (type)
                {
                    case kNull: {
                        // Read the null parameter
                        reader.readNull();
                        ss << "Null";
                    } break;

                    case kBoolFalse:
                    case kBoolTrue: {
                        // Read the bool parameter
                        bool value;
                        reader.read(value);
                        ss << (value) ? "True" : "False";
                    } break;

                    case kPosFixInt:
                    case kUint8:
                    case kUint16:
                    case kUint32:
                    case kUint64: {
                        // Read the unsigned integer parameter
                        uint64_t value;
                        reader.read(value);
                        // Print it with '0x' and leading 0's
                        ss << value << " 0x" << std::setfill('0') << std::setw(getWidth(value)) << value << std::setfill(' ');
                    } break;

                    case kNegFixInt:
                    case kInt8:
                    case kInt16:
                    case kInt32:
                    case kInt64: {
                        // Read the integer parameter
                        int64_t value;
                        reader.read(value);
                        ss << value;
                        if (type == kInt8 && value >= 0x20 && value <= 0x7E) // If the int is a printable char
                        {
                            ss << " '" << static_cast<int8_t>(type) << "'";
                        }
                    } break;

                    case kFloat: {
                        // Read the float parameter
                        float value;
                        reader.read(value);
                        ss << value;
                    } break;

                    case kError: {
                        // Read the error code
                        uint8_t code;
                        reader.readError(code);
                        ss << +code << " 0x" << std::hex << +code;

                        // Read the associated data
                        int16_t data;
                        reader.read(data);
                        ss << " " << data;
                    } break;

                    case kEndOfMessage: {
                        // Read end of message and check the CRC
                        ss.str("");
                        ss << (m_type == BufferType::Write ? "Write" : "Read ") << " Message " << message_id << " CRC " << (reader.readCrc() ? "Valid" : "Invalid");
                    } break;

                    default: {
                        // Most likely a rogue print statement in the device code
                        ss << "Unknown Type";
                    } break;
                }
                m_print_func(ss.str());
                ++param;
            }
        }

        void DebugBuffer::DecodeBuffer::writeByte(const uint8_t byte)
        {
            m_buf.emplace_back(byte);
            if (byte == emb::shared::DataType::kEndOfMessage)
            {
                m_receivedEOM = true;
            }
            else if (m_receivedEOM)
            {
                ++m_message_count;
                m_receivedEOM = false;
                decode(); // Received full message, decode it.
            }
        }

        uint8_t DebugBuffer::DecodeBuffer::peek() const
        {
            return m_buf.at(0);
        }

        uint8_t DebugBuffer::DecodeBuffer::readByte()
        {
            uint8_t byte = m_buf.at(0);
            m_buf.erase(m_buf.begin());
            if (byte == emb::shared::DataType::kEndOfMessage)
            {
                m_readEOM = true;
            }
            else if (m_readEOM)
            {
                --m_message_count;
                m_readEOM = false;
            }
            return byte;
        }

        bool DebugBuffer::DecodeBuffer::empty() const
        {
            return m_buf.empty();
        }

        size_t DebugBuffer::DecodeBuffer::size() const
        {
            return m_buf.size();
        }

        uint8_t DebugBuffer::DecodeBuffer::messages() const
        {
            return m_message_count;
        }

        void DebugBuffer::DecodeBuffer::update()
        {
        }

        void DebugBuffer::DecodeBuffer::zero()
        {
            m_buf.clear();
            m_receivedEOM = false;
            m_readEOM = false;
            m_message_count = 0;
        }

        DebugBuffer::DebugBuffer(emb::shared::IBuffer* buffer, std::function<void(std::string)> print_func) :
            m_real_buffer(buffer),
            m_read_buffer(BufferType::Read, print_func),
            m_write_buffer(BufferType::Write, print_func)
        {
        }
        
        // Copy the byte into the write decode buffer, then forward it to the real buffer
        void DebugBuffer::writeByte(const uint8_t byte)
        {
            m_write_buffer.writeByte(byte);
            m_real_buffer->writeByte(byte);
        }

        // Use the real buffer's peek
        uint8_t DebugBuffer::peek() const
        {
            return m_real_buffer->peek();
        }

        // Read the byte from the real buffer, copy it into the read decode buffer, then forward it
        uint8_t DebugBuffer::readByte()
        {
            uint8_t byte = m_real_buffer->readByte();
            m_read_buffer.writeByte(byte);
            return byte;
        }

        // Use the real buffer's empty
        bool DebugBuffer::empty() const
        {
            return m_real_buffer->empty();
        }

        // Use the real buffer's size
        size_t DebugBuffer::size() const
        {
            return m_real_buffer->size();
        }

        // Use the real buffer's message count
        uint8_t DebugBuffer::messages() const
        {
            return m_real_buffer->messages();
        }

        // Update the real buffer
        void DebugBuffer::update()
        {
            m_real_buffer->update();
        }

        // Zero out all buffers
        void DebugBuffer::zero()
        {
            m_read_buffer.zero();
            m_write_buffer.zero();
            m_real_buffer->zero();
        }
    }  // namespace host
}  // namespace emb
