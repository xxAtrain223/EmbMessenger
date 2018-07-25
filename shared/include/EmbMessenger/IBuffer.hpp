#ifndef EMBMESSENGER_IBUFFER_HPP
#define EMBMESSENGER_IBUFFER_HPP

#include <stdint.h>
#include <stddef.h>

namespace emb
{
    /**
     * @brief Interface for managing the device I/O.
     */
    class IBuffer
    {
    public:
        /**
         * @brief Writes the @p byte to the end of the buffer.
         *
         * @param byte Value to be written
         */
        virtual void writeByte(const uint8_t byte) = 0;

        /**
         * @brief Gets the byte from the front of the buffer, without removing it.
         *
         * @returns Byte at the front of the buffer
         */
        virtual uint8_t peek() const = 0;

        /**
         * @brief Reads the byte from the front of the buffer,
         * it does get removed.
         *
         * @returns Byte at the front of the buffer
         */
        virtual uint8_t readByte() = 0;

        /**
         * @brief Checks if the buffer is empty.
         *
         * @returns True if the buffer is empty
         */
        virtual bool empty() const = 0;

        /**
         * @brief Gets the number of bytes in the buffer.
         *
         * @returns Number of bytes in the buffer
         */
        virtual size_t size() const = 0;

        /**
         * @brief Checks if the buffer has a message available.
         *
         * @returns True if the buffer has a message available
         */
        virtual bool messageAvailable() const = 0;

        /**
         * @brief Updates the internal buffer.
         */
        virtual void update() = 0;
    };
}
#endif // EMBMESSENGER_IBUFFER_HPP