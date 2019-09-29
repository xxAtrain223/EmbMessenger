#include "FakeBuffer.hpp"
#include "EmbMessenger/Crc.hpp"
#include "EmbMessenger/DataType.hpp"

#include <algorithm>

namespace emb
{
    namespace host
    {
        namespace test
        {
            void FakeBuffer::addDeviceMessage(std::vector<uint8_t>&& message)
            {
                uint8_t crc = 0;

                auto addByte = [&](uint8_t byte) {
                    device.emplace_back(byte);
                    crc = shared::crc::Calculate8(crc, byte);
                };

                for (uint8_t byte : message)
                {
                    addByte(byte);
                }
                addByte(shared::DataType::kEndOfMessage);
                addByte(crc + !validCrc);

                ++deviceMessages;
            }

            bool FakeBuffer::checkHostBuffer(std::vector<uint8_t>&& message)
            {
                uint8_t crc = 0;

                for (uint8_t byte : message)
                {
                    crc = shared::crc::Calculate8(crc, byte);
                }
                message.emplace_back(shared::DataType::kEndOfMessage);
                crc = shared::crc::Calculate8(crc, shared::DataType::kEndOfMessage);
                message.emplace_back(crc);

                bool rv = host == message;

                if (host.size() >= message.size())
                {
                    host.erase(std::begin(host), std::begin(host) + message.size());
                }
                else
                {
                    host.erase(std::begin(host), std::begin(host) + host.size());
                }

                return rv;
            }

            bool FakeBuffer::buffersEmpty()
            {
                return host.empty() && device.empty();
            }

            void FakeBuffer::writeValidCrc(bool value)
            {
                validCrc = value;
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

                if (byte == shared::DataType::kEndOfMessage)
                {
                    readCrc = true;
                }
                else if (readCrc == true)
                {
                    readCrc = false;
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

            uint8_t FakeBuffer::messages() const
            {
                return deviceMessages;
            }

            void FakeBuffer::update()
            {
                (void)0;  // Noop
            }

            void FakeBuffer::zero()
            {
                deviceMessages = 0;
                device.clear();
                readCrc = false;
            }
        }  // namespace test
    }  // namespace host
}  // namespace emb
