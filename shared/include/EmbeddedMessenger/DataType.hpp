#ifndef EMBEDDEDMESSENGER_DATATYPE_HPP
#define EMBEDDEDMESSENGER_DATATYPE_HPP

#include <cstdint>

namespace emb
{
    /**
     * @brief Describes the type of the data in the next few bytes of the message.
     */
    enum DataType : uint8_t
    {
        kNull = 0xC0,
        kBoolFalse = 0xC2,
        kBoolTrue = 0xC3,
        kFloat = 0xCA,
        kUint8 = 0xCC,
        kUint16 = 0xCD,
        kUint32 = 0xCE,
        kUint64 = 0xCF,
        kInt8 = 0xD0,
        kInt16 = 0xD1,
        kInt32 = 0xD2,
        kInt64 = 0xD3,
        kPosFixInt = 0x00,
        kNegFixInt = 0xE0,
        kCrc = 0xC1,
        kError = 0xCB
    };
}

#endif // EMBEDDEDMESSENGER_DATATYPE_HPP