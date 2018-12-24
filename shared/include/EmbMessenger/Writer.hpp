#ifndef EMBMESSENGER_WRITER_HPP
#define EMBMESSENGER_WRITER_HPP

#include <stdint.h>

#include "EmbMessenger/Templates.hpp"

namespace emb
{
    namespace shared
    {
        class IBuffer;
        enum DataError : uint8_t;

        /**
         * @brief Message writer, used to serialize messages.
         */
        class Writer
        {
        protected:
            IBuffer* m_buffer;
            uint8_t m_crc;

            /**
             * @brief Writes the byte to the buffer and updates the CRC.
             *
             * @param byte Byte to write to the buffer
             */
            void writeByte(const uint8_t byte);

            /**
             * @brief Writes the @p value to the buffer and updates the crc.
             *
             * @param value Data to write to the buffer
             */
            template <typename T>
            void writeData(const T value)
            {
                union {
                    T in;
                    var_uint_t<T> out;  // uint the size of T
                } u;
                u.in = value;

                for (int8_t i = sizeof(T) - 1; i >= 0; --i)
                {
                    writeByte(u.out >> (i * 8) & 0xFF);
                }
            }

        public:
            /**
             * @brief Constructs a buffer writer using the @p buffer.
             *
             * @param buffer Buffer to use for communicating
             */
            Writer(IBuffer* buffer);

            /**
             * @brief Writes a `bool` to the buffer.
             *
             * @param value `bool` to write to the buffer
             */
            void write(const bool value);

            /**
             * @brief Writes a `uint8_t` to the buffer.
             *
             * @param value `uint8_t` to write to the buffer
             */
            void write(const uint8_t value);

            /**
             * @brief Writes a `uint16_t` to the buffer.
             *
             * @param value `uint16_t` to write to the buffer
             */
            void write(const uint16_t value);

            /**
             * @brief Writes a `uint32_t` to the buffer.
             *
             * @param value `uint32_t` to write to the buffer
             */
            void write(const uint32_t value);

            /**
             * @brief Writes a `uint64_t` to the buffer.
             *
             * @param value `uint64_t` to write to the buffer
             */
            void write(const uint64_t value);

            /**
             * @brief Writes a `int8_t` to the buffer.
             *
             * @param value `int8_t` to write to the buffer
             */
            void write(const int8_t value);

            /**
             * @brief Writes a `int16_t` to the buffer.
             *
             * @param value `int16_t` to write to the buffer
             */
            void write(const int16_t value);

            /**
             * @brief Writes a `int32_t` to the buffer.
             *
             * @param value `int32_t` to write to the buffer
             */
            void write(const int32_t value);

            /**
             * @brief Writes a `int64_t` to the buffer.
             *
             * @param value `int64_t` to write to the buffer
             */
            void write(const int64_t value);

            /**
             * @brief Writes a `float` to the buffer.
             *
             * @param value `float` to write to the buffer
             */
            void write(const float value);

            /**
             * @brief Writes a `null` value to the buffer.
             */
            void writeNull();

            /**
             * @brief Writes an error byte to the buffer.
             *
             * @param value Error to write to the buffer
             */
            void writeError(const DataError value);

            /**
             * @brief Writes the CRC to the buffer.
             *        This should be used to end a message.
             */
            void writeCrc();
        };
    }  // namespace shared
}  // namespace emb

#endif  // EMBMESSENGER_WRITER_HPP