#ifndef EMBMESSENGER_DATAERROR_HPP
#define EMBMESSENGER_DATAERROR_HPP

#include <cstdint>

namespace emb
{
    /**
     * @brief Describes errors that may occur on the device.
     */
    enum DataError : uint8_t
    {
        kExtraParameters = 0x00,
        kOutOFPeriodicCommandSlots = 0x01,

        kParameter0ReadError = 0x10,
        kParameter1ReadError = 0x11,
        kParameter2ReadError = 0x12,
        kParameter3ReadError = 0x13,
        kParameter4ReadError = 0x14,
        kParameter5ReadError = 0x15,
        kParameter6ReadError = 0x16,
        kParameter7ReadError = 0x17,
        kMessageIdReadError = 0x18,
        kCommandIdReadError = 0x19,
        kCrcReadError = 0x1A,

        kParameter0Invalid = 0x20,
        kParameter1Invalid = 0x21,
        kParameter2Invalid = 0x22,
        kParameter3Invalid = 0x23,
        kParameter4Invalid = 0x24,
        kParameter5Invalid = 0x25,
        kParameter6Invalid = 0x26,
        kParameter7Invalid = 0x27,
        kMessageIdInvalid = 0x28,
        kCommandIdInvalid = 0x29,
        kCrcInvalid = 0x2A
    };
}

#endif // EMBMESSENGER_DATAERROR_HPP