#ifndef EMBMESSENGER_DATAERROR_HPP
#define EMBMESSENGER_DATAERROR_HPP

#include <stdint.h>

namespace emb
{
    /**
     * @brief Describes errors that may occur on the device.
     */
    enum DataError : uint8_t
    {
        // Cannot use 0x00 because of longjmp
        kExtraParameters = 0x01,
        kOutOfPeriodicCommandSlots = 0x02,

        kParameterReadError = 0x10,
        kMessageIdReadError = 0x11,
        kCommandIdReadError = 0x12,
        kCrcReadError = 0x13,

        kParameterInvalid = 0x18,
        kMessageIdInvalid = 0x19,
        kCommandIdInvalid = 0x1A,
        kCrcInvalid = 0x1B
    };
}

#endif // EMBMESSENGER_DATAERROR_HPP