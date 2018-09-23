#include "FakeBuffer.hpp"
#include "EmbMessenger/DataType.hpp"
#include "EmbMessenger/Crc.hpp"

#include <algorithm>
#include <cstdio>

namespace emb
{
    namespace test
    {
        void FakeBuffer::addHostMessage(std::vector<uint8_t>&& message)
        {
            uint8_t crc = 0;

            auto addByte = [&](uint8_t byte) {
                host.emplace_back(byte);
                crc = crc::Calculate8(crc, byte);
            };

            for (uint8_t byte : message)
            {
                addByte(byte);
            }
            addByte(DataType::kCrc);

            if (appendCrc)
            {
                addByte(crc + !validCrc);
            }

            ++hostMessages;
        }

        bool FakeBuffer::checkDeviceBuffer(std::vector<uint8_t>&& message)
        {
            uint8_t crc = 0;

            for (uint8_t byte : message)
            {
                crc = crc::Calculate8(crc, byte);
            }
            message.emplace_back(DataType::kCrc);
            crc = crc::Calculate8(crc, DataType::kCrc);
            message.emplace_back(crc);

            bool rv = device == message;

            device.erase(std::begin(device), std::begin(device) + ((device.size() < message.size()) ? device.size() : message.size()));

            return rv;
        }

        bool FakeBuffer::buffersEmpty()
        {
            return host.empty() && device.empty();
        }

        void FakeBuffer::writeCrc(bool value)
        {
            appendCrc = value;
        }

        void FakeBuffer::writeValidCrc(bool value)
        {
            validCrc = value;
        }

        void FakeBuffer::printBuffers()
        {
            printf("Device:");
            for (uint8_t byte : device)
            {
                printf(" 0x%02X", byte);
            }

            printf("\nHost:");
            for (uint8_t byte : host)
            {
                printf(" 0x%02X", byte);
            }

            printf("\n");
        }

        void FakeBuffer::writeByte(const uint8_t byte)
        {
            device.emplace_back(byte);
        }

        uint8_t FakeBuffer::peek() const
        {
            return host.front();
        }

        uint8_t FakeBuffer::readByte()
        {
            uint8_t byte = host.front();
            host.erase(std::begin(host));

            if (byte == DataType::kCrc)
            {
                readCrc = true;
            }
            else if (readCrc == true)
            {
                readCrc = false;
                --hostMessages;
            }

            return byte;
        }

        bool FakeBuffer::empty() const
        {
            return host.empty();
        }

        size_t FakeBuffer::size() const
        {
            return host.size();
        }

        uint8_t FakeBuffer::messagesAvailable() const
        {
            return hostMessages;
        }

        void FakeBuffer::update()
        {
            (void)0; // Noop
        }
    }
}
