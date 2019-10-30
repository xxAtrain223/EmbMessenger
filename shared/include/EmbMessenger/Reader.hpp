#ifndef EMBMESSENGER_READER_HPP
#define EMBMESSENGER_READER_HPP

#include <stdint.h>

#include "Crc.hpp"
#include "IBuffer.hpp"
#include "Templates.hpp"

namespace emb
{
    namespace shared
    {
        enum DataType : uint8_t;
        enum DataError : uint8_t;

        /**
         * @brief Message reader, used to deserialize messages.
         */
        class Reader
        {
        protected:
            IBuffer* m_buffer;
            uint8_t m_crc;

            /**
             * @brief Peeks at the next byte in the buffer.
             * This will not remove the byte from the buffer.
             *
             * @param[out] value Will contain the next byte in the buffer
             *
             * @returns True if successful
             */
            bool peekByte(uint8_t& value) const;

            /**
             * @brief Reads the next byte in the buffer.
             * This will remove the byte from the buffer.
             *
             * @param[out] value Will contain the next byte in the buffer
             *
             * @returns True if successful
             */
            bool readByte(uint8_t& value);

            /**
             * @brief Reads the bytes from the buffer into @p value.
             * This will remove sizeof(T) bytes from the buffer
             *
             * @param[out] value Will be populated with the bytes from the stream
             *
             * @returns True if the buffer contains enough bytes to populate @p value
             */
            template <typename T>
            bool readData(T& value)
            {
                if (m_buffer->size() < sizeof(T))
                {
                    return false;
                }

                union {
                    var_uint_t<T> in;  // uint the size of T
                    T out;
                } u;
                u.in = 0;

                for (int8_t i = sizeof(T) - 1; i >= 0; --i)
                {
                    uint8_t byte = m_buffer->readByte();
                    m_crc = crc::Calculate8(m_crc, byte);
                    u.in |= static_cast<var_uint_t<T>>(byte) << (i * 8);
                }

                value = u.out;
                return true;
            }

            /**
             * @brief Removes the next byte from the buffer.
             * Wrapper for getByte
             *
             * @returns True if successful
             */
            bool removeByte();

        public:
            /**
             * @brief Constructs a message reader using the @p buffer.
             *
             * @param buffer Buffer to use for communicating
             */
            Reader(IBuffer* buffer);

            /**
             * @brief Gets the type of the next parameter in the buffer.
             * This will not remove the byte from the buffer.
             *
             * @param[out] type The type of the next parameter in the buffer
             *
             * @returns True if the type was read successfully
             */
            bool getType(DataType& type) const;

            /**
             * @brief Checks if the next parameter in the buffer is a bool.
             * This will not remove the byte from the buffer.
             *
             * @returns True if the next parameter in the buffer is a bool
             */
            bool nextBool() const;

            /**
             * @brief Checks if the next parameter in the buffer is a char.
             * This will not remove the byte from the buffer.
             *
             * @returns True if the next parameter in the buffer is a char
             */
            bool nextChar() const;

            /**
             * @brief Checks if the next parameter in the buffer is a signed int.
             * This will not remove the byte from the buffer.
             *
             * @returns True if the next parameter in the buffer is a signed int
             */
            bool nextSignedInt() const;

            /**
             * @brief Checks if the next parameter in the buffer is an unsigned int.
             * This will not remove the byte from the buffer.
             *
             * @returns True if the next parameter in the buffer is an unsigned int
             */
            bool nextUnsignedInt() const;

            /**
             * @brief Checks if the next parameter in the buffer is a float.
             * This will not remove the byte from the buffer.
             *
             * @returns True if the next parameter in the buffer is a float
             */
            bool nextFloat() const;

            /**
             * @brief Checks if the next parameter in the buffer is null (nil).
             * This will not remove the byte from the buffer.
             *
             * @returns True if the next parameter in the buffer is null
             */
            bool nextNull() const;

            /**
             * @brief Checks if the next parameter in the buffer is an error.
             * This will not remove the byte from the buffer.
             *
             * @returns True if the next parameter in the buffer is an error
             */
            bool nextError() const;

            /**
             * @brief Checks if the next parameter in the buffer is the CRC.
             * This will not remove the byte from the buffer.
             *
             * @returns True if the next parameter in the buffer is the CRC
             */
            bool nextCrc() const;

            /**
             * @brief Attempts to read the next parameter in the buffer as `bool`.
             * This will remove the `bool` byte if successful.
             *
             * @param[out] value Will contain the next `bool` parameter from the buffer
             *
             * @returns True if the next parameter is `bool` and was read successfully
             */
            bool read(bool& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `uint8_t`.
             * This will remove the `uint8_t` byte if successful.
             *
             * @param[out] value Will contain the next `uint8_t` parameter from the buffer
             *
             * @returns True if the next parameter is `uint8_t` and was read successfully
             */
            bool read(uint8_t& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `uint16_t`.
             * This will remove the `uint16_t` bytes if successful.
             *
             * @param[out] value Will contain the next `uint16_t` parameter from the buffer
             *
             * @returns True if the next parameter is `uint16_t` and was read successfully
             */
            bool read(uint16_t& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `uint32_t`.
             * This will remove the `uint32_t` bytes if successful.
             *
             * @param[out] value Will contain the next `uint32_t` parameter from the buffer
             *
             * @returns True if the next parameter is `uint32_t` and was read successfully
             */
            bool read(uint32_t& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `uint64_t`.
             * This will remove the `uint64_t` bytes if successful.
             *
             * @param[out] value Will contain the next `uint64_t` parameter from the buffer
             *
             * @returns True if the next parameter is `uint64_t` and was read successfully
             */
            bool read(uint64_t& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `int8_t`.
             * This will remove the `int8_t` byte if successful.
             *
             * @param[out] value Will contain the next `int8_t` parameter from the buffer
             *
             * @returns True if the next parameter is `int8_t` and was read successfully
             */
            bool read(int8_t& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `int16_t`.
             * This will remove the `int16_t` bytes if successful.
             *
             * @param[out] value Will contain the next `int16_t` parameter from the buffer
             *
             * @returns True if the next parameter is `int16_t` and was read successfully
             */
            bool read(int16_t& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `int32_t`.
             * This will remove the `int32_t` bytes if successful.
             *
             * @param[out] value Will contain the next `int32_t` parameter from the buffer
             *
             * @returns True if the next parameter is `int32_t` and was read successfully
             */
            bool read(int32_t& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `int64_t`.
             * This will remove the `int64_t` bytes if successful.
             *
             * @param[out] value Will contain the next `int64_t` parameter from the buffer
             *
             * @returns True if the next parameter is `int64_t` and was read successfully
             */
            bool read(int64_t& value);

            /**
             * @brief Attempts to read the next parameter in the buffer as `float`.
             * This will remove the `float` bytes if successful.
             *
             * @param[out] value Will contain the next `float` parameter from the buffer
             *
             * @returns True if the next parameter is `float` and was read successfully
             */
            bool read(float& value);

            /**
             * @brief Attempts to read the next paramter in the buffer as `null`.
             * This will remove the `null` byte if successful.
             *
             * @returns True if the next parameter is null and was read successfully
             */
            bool readNull();

            /**
             * @brief Attempts to read the next parameter in the buffer as an error.
             * This will remove the error bytes if successful.
             *
             * @param[out] value Will contain the next error parameter from the buffer
             *
             * @returns True if the next parameter is an error and was read successfully
             */
            bool readError(uint8_t& value);

            /**
             * @brief Reads the CRC and checks if it's valid.
             * This will remove the CRC bytes if successful.
             *
             * @returns True if the CRC is valid.
             */
            bool readCrc();

            /**
             * @brief Resets the internal CRC.
             * Useful when bytes are removed from buffer outside of this Reader.
             */
            void resetCrc();

        protected:
            // Helper functions

            template <typename ReadType, typename ConvertType>
            inline bool readAndConvert(ConvertType& ct)
            {
                ReadType rt = 0;
                bool rv = read(rt);
                ct = rt;
                return rv;
            }

            template <typename T>
            inline bool removeTypeAndRead(T& t)
            {
                if (m_buffer->size() >= sizeof(T) + 1)
                {
                    removeByte();  // Remove Type Byte
                    return readData(t);
                }
                return false;
            }
        };
    }  // namespace shared
}  // namespace emb

#endif  // EMBMESSENGER_READER_HPP