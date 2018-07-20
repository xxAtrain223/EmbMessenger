#ifndef EMBEDDEDMESSENGER_CRC_HPP
#define EMBEDDEDMESSENGER_CRC_HPP

#include <cstdint>
#include <cstddef>

#if !defined(CRC8) && !defined(CRC16) && !defined(CRC32)
#define CRC8
#define CRC16
#define CRC32
#endif

namespace emb
{
    namespace crc
    {
        namespace detail
        {
            template<typename T>
            constexpr size_t nBits()
            {
                return ((sizeof(T) - 1) * 8);
            }

            template<typename T, T generator>
            constexpr T computek(T c, uint8_t k = 0) {
                return k < 8 ? computek<T, generator>((c & (0x80UL << nBits<T>())) ? (generator ^ (c << 1)) : (c << 1), k + 1) : c;
            }

            template<typename T>
            struct CrcTable {
                T data[256];
            };

            template<typename T, bool> struct sfinae { typedef CrcTable<T> type; };
            template<typename T> struct sfinae<T, false> { };

            template<typename T, T generator, typename ...Ts>
            constexpr typename sfinae<T, sizeof...(Ts) == 256>::type compute(T n, Ts... t) {
                return CrcTable<T>{ { t... } };
            }

            template<typename T, T generator, typename ...Ts>
            constexpr typename sfinae<T, sizeof...(Ts) <= 255>::type compute(T n, Ts... t) {
                return compute<T, generator>(n + 1, t..., computek<T, generator>(n << nBits<T>()));
            }

            template<typename T>
            T Calculate(const T table[], const T& crc, const uint8_t& data)
            {
                /* XOR-in next input byte */
                uint8_t pos = (uint8_t)((crc ^ ((uint32_t)data << nBits<T>())) >> nBits<T>());

                /* get current CRC value = remainder */
                return ((sizeof(T) > 1) ? (crc << 8) : 0) ^ table[pos];
            }

            #ifdef CRC8
            extern const CrcTable<uint8_t> CrcTable8;
            #endif
            #ifdef CRC16
            extern const CrcTable<uint16_t> CrcTable16;
            #endif
            #ifdef CRC32
            extern const CrcTable<uint32_t> CrcTable32;
            #endif
        }

        #ifdef CRC8
        inline uint8_t Calculate8(const uint8_t& crc, const uint8_t& data)
        {
            return detail::Calculate(detail::CrcTable8.data, crc, data);
        }
        #endif

        #ifdef CRC16
        inline uint16_t Calculate16(const uint16_t& crc, const uint8_t& data)
        {
            return detail::Calculate(detail::CrcTable16.data, crc, data);
        }
        #endif

        #ifdef CRC32
        inline uint32_t Calculate32(const uint32_t& crc, const uint8_t& data)
        {
            return detail::Calculate(detail::CrcTable32.data, crc, data);
        }
        #endif
    }
}

#endif // EMBEDDEDMESSENGER_CRC_HPP