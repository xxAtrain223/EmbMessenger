#include "FakeBuffer.hpp"
#include "EmbMessenger/Crc.hpp"
#include "EmbMessenger/DataType.hpp"

#include <algorithm>
#include <cstdio>

namespace emb
{
    namespace device
    {
        namespace test
        {
            void FakeBuffer::addHostMessage(std::vector<uint8_t>&& message)
            {
                uint8_t crc = 0;

                auto addByte = [&](uint8_t byte) {
                    host.emplace_back(byte);
                    crc = shared::crc::Calculate8(crc, byte);
                };

                for (uint8_t byte : message)
                {
                    addByte(byte);
                }
                addByte(shared::DataType::kCrc);

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
                    crc = shared::crc::Calculate8(crc, byte);
                }
                message.emplace_back(shared::DataType::kCrc);
                crc = shared::crc::Calculate8(crc, shared::DataType::kCrc);
                message.emplace_back(crc);

                bool rv = std::equal(std::begin(message), std::end(message), std::begin(device));

                device.erase(std::begin(device),
                             std::begin(device) + ((device.size() < message.size()) ? device.size() : message.size()));

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

            void FakeBuffer::print()
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

                if (byte == shared::DataType::kCrc)
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

            uint8_t FakeBuffer::messages() const
            {
                return hostMessages;
            }

            void FakeBuffer::update()
            {
                (void)0;  // Noop
            }

            void FakeBuffer::zero()
            {
                hostMessages = 0;
                host.clear();
                readCrc = false;
            }
        }  // namespace test
    }  // namespace device
}  // namespace emb
