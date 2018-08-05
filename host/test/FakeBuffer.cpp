#include "FakeBuffer.hpp"
#include "EmbMessenger/DataType.hpp"
#include "EmbMessenger/Crc.hpp"

#include <algorithm>

namespace emb
{
    namespace test
    {
        void FakeBuffer::addDeviceMessage(std::vector<uint8_t>&& message)
        {
            uint8_t crc = 0;

            auto addByte = [&](uint8_t byte) {
                device.emplace_back(byte);
                crc = crc::Calculate8(crc, byte);
            };

            for (uint8_t byte : message)
            {
                addByte(byte);
            }
            addByte(DataType::kCrc);
            addByte(crc);

            ++deviceMessages;
        }

        bool FakeBuffer::checkHostBuffer(std::vector<uint8_t>&& message)
        {
            uint8_t crc = 0;

            for (uint8_t byte : message)
            {
                crc = crc::Calculate8(crc, byte);
            }
            message.emplace_back(DataType::kCrc);
            crc = crc::Calculate8(crc, DataType::kCrc);
            message.emplace_back(crc);

            bool rv = host == message;

            host.erase(std::begin(host), std::begin(host) + message.size());

            return rv;
        }

        bool FakeBuffer::buffersEmpty()
        {
            return host.empty() && device.empty();
        }

        void FakeBuffer::writeByte(const uint8_t byte)
        {
            host.emplace_back(byte);
        }

        uint8_t FakeBuffer::peek() const
        {
            return device.front();
        }

        uint8_t FakeBuffer::readByte()
        {
            uint8_t byte = device.front();
            device.erase(std::begin(device));

            if (byte == DataType::kCrc)
            {
                // In reality, the byte after kCrc is the end of the message
                --deviceMessages;
            }

            return byte;
        }

        bool FakeBuffer::empty() const
        {
            return device.empty();
        }

        size_t FakeBuffer::size() const
        {
            return device.size();
        }

        uint8_t FakeBuffer::messagesAvailable() const
        {
            return deviceMessages;
        }

        void FakeBuffer::update()
        {
            (void)0; // Noop
        }
    }
}