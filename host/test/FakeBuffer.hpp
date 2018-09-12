#ifndef EMBMESSENGER_TEST_FAKEBUFFER_HPP
#define EMBMESSENGER_TEST_FAKEBUFFER_HPP

#include "EmbMessenger/IBuffer.hpp"

#include <cstdint>
#include <vector>

namespace emb
{
    namespace test
    {
        class FakeBuffer : public IBuffer
        {
        protected:
            std::vector<uint8_t> host;
            std::vector<uint8_t> device;
            int deviceMessages = 0;
            bool readCrc = false;
            bool validCrc = true;

        public:
            /**
             * @brief Appends a message to the device buffer, auto adds crc.
             *   Used for adding a message to be read by the host.
             *
             * @param message Message to append
             */
            void addDeviceMessage(std::vector<uint8_t>&& message);

            /**
             * @brief Checks if the message is in the host buffer, auto adds crc.
             *   Used for checking if the host wrote what was expected.
             *   Removes the number of elements in message from the host buffer.
             *
             * @param message Message to check for in the host buffer
             *
             * @returns True if the message was in the host buffer
             */
            bool checkHostBuffer(std::vector<uint8_t>&& message);

            bool buffersEmpty();

            void writeValidCrc(const bool value);

            virtual void writeByte(const uint8_t byte) override;
            virtual uint8_t peek() const override;
            virtual uint8_t readByte() override;
            virtual bool empty() const override;
            virtual size_t size() const override;
            virtual uint8_t messagesAvailable() const override;
            virtual void update() override;
        };
    }
}

#endif // EMBMESSENGER_TEST_FAKEBUFFER_HPP
