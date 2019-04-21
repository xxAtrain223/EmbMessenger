#ifndef DEBUGBUFFER_HPP
#define DEBUGBUFFER_HPP

#include <functional>
#include <string>
#include <iostream>
#include <vector>

#include "EmbMessenger/IBuffer.hpp"
#include "EmbMessenger/Reader.hpp"

namespace emb
{
    namespace host
    {
        /**
         * @brief Buffer for debugging what gets sent to and from the device.
         * Intercepts, interprets and forwards bytes sent to the device.
         */
        class DebugBuffer : public emb::shared::IBuffer
        {
            enum class BufferType
            {
                Write,
                Read
            };

            class DecodeBuffer : public emb::shared::IBuffer
            {
                std::vector<uint8_t> m_buf;
                bool m_receivedEOM = false, m_readEOM = false;
                uint8_t m_message_count = 0;
                BufferType m_type;
                std::function<void(std::string)> m_print_func;

                void decode();

            public:
                DecodeBuffer(BufferType type, std::function<void(std::string)> print_func);

                virtual void writeByte(const uint8_t byte) override;
                virtual uint8_t peek() const override;
                virtual uint8_t readByte() override;
                virtual bool empty() const override;
                virtual size_t size() const override;
                virtual uint8_t messages() const override;
                virtual void update() override;
                virtual void zero() override;
            };

            emb::shared::IBuffer* m_real_buffer;
            DecodeBuffer m_read_buffer, m_write_buffer;

        public:
            /**
             * @brief Constructor to setup the internal buffers.
             *
             * @param buffer The real buffer to use for communication.
             * @param print_func The function for printing the output. Defaults to stdout.
             */
            DebugBuffer(emb::shared::IBuffer* buffer, std::function<void(std::string)> print_func = [](std::string str) { std::cout << str << std::endl; });

            virtual void writeByte(const uint8_t byte) override;
            virtual uint8_t peek() const override;
            virtual uint8_t readByte() override;
            virtual bool empty() const override;
            virtual size_t size() const override;
            virtual uint8_t messages() const override;
            virtual void update() override;
            virtual void zero() override;
        };
    }  // namespace host
}  // namespace emb

#endif  // DEBUGBUFFER_HPP