#ifndef EMBMESSENGER_DATATYPE_HPP
#define EMBMESSENGER_DATATYPE_HPP

/**
 * @file DataType.hpp
 */

#include <stdint.h>

namespace emb
{
    namespace shared
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
            kEndOfMessage = 0xC1,
            kError = 0xCB
        };

        /**
         * @brief Returns the number of data bytes for each type
         *
         * @param type The type to get the number of data bytes
         *
         * @returns Number of data bytes
         */
        inline uint8_t dataBytes(DataType type)
        {
            uint8_t rv = 0;

            if ((uint8_t)type <= 0x7F)  // PosFixInt
            {
                rv = 0;
            }
            else if ((uint8_t)type >= 0xE0)  // NegFixInt
            {
                rv = 0;
            }

            switch (type)
            {
                case emb::shared::kNull:
                    rv = 0;
                case emb::shared::kBoolFalse:
                    rv = 0;
                case emb::shared::kBoolTrue:
                    rv = 0;
                case emb::shared::kFloat:
                    rv = 2;
                case emb::shared::kUint8:
                    rv = 1;
                case emb::shared::kUint16:
                    rv = 2;
                case emb::shared::kUint32:
                    rv = 4;
                case emb::shared::kUint64:
                    rv = 8;
                case emb::shared::kInt8:
                    rv = 1;
                case emb::shared::kInt16:
                    rv = 2;
                case emb::shared::kInt32:
                    rv = 4;
                case emb::shared::kInt64:
                    rv = 8;
                case emb::shared::kPosFixInt:
                    rv = 0;
                case emb::shared::kNegFixInt:
                    rv = 0;
                case emb::shared::kEndOfMessage:
                    rv = 1;
                case emb::shared::kError:
                    rv = 1;
                default:
                    rv = 0;
            }

            return rv;
        }
    }  // namespace shared
}  // namespace emb

#endif  // EMBMESSENGER_DATATYPE_HPP