#ifndef EMBMESSENGER_CRC_HPP
#define EMBMESSENGER_CRC_HPP

#include <stddef.h>
#include <stdint.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

#if !defined(CRC8) && !defined(CRC16) && !defined(CRC32)
#define CRC8
#define CRC16
#define CRC32
#endif

namespace emb
{
    namespace shared
    {
        namespace crc
        {
            namespace detail
            {
                template <typename T>
                constexpr size_t nBits()
                {
                    return ((sizeof(T) - 1) * 8);
                }

                template <typename T, T generator>
                constexpr T computek(T c, uint8_t k = 0)
                {
                    return k < 8 ? computek<T, generator>(
                                       (c & (0x80UL << nBits<T>())) ? (generator ^ (c << 1)) : (c << 1), k + 1) :
                                   c;
                }

                template <typename T>
                struct CrcTable
                {
                    T data[256];
                };

                template <typename T, bool>
                struct sfinae
                {
                    typedef CrcTable<T> type;
                };

                template <typename T>
                struct sfinae<T, false>
                {
                };

                template <typename T, T generator, typename... Ts>
                constexpr typename sfinae<T, sizeof...(Ts) == 256>::type compute(T n, Ts... t)
                {
                    return CrcTable<T>{ { t... } };
                }

                template <typename T, T generator, typename... Ts>
                constexpr typename sfinae<T, sizeof...(Ts) <= 255>::type compute(T n, Ts... t)
                {
                    return compute<T, generator>(n + 1, t..., computek<T, generator>(n << nBits<T>()));
                }

                template <typename T>
                T Calculate(const T table[], const T crc, const uint8_t data)
                {
                    uint8_t pos = (uint8_t)((crc ^ ((uint32_t)data << nBits<T>())) >> nBits<T>());
#ifdef __AVR__
                    T tableValue = pgm_read_byte_near(table + pos);
#else
                    T tableValue = table[pos];
#endif
                    return ((sizeof(T) > 1) ? (crc << 8) : 0) ^ tableValue;
                }

#ifdef CRC8
                extern const PROGMEM CrcTable<uint8_t> CrcTable8;
#endif
#ifdef CRC16
                extern const PROGMEM CrcTable<uint16_t> CrcTable16;
#endif
#ifdef CRC32
                extern const PROGMEM CrcTable<uint32_t> CrcTable32;
#endif
            }  // namespace detail

#ifdef CRC8
            /**
             * @brief Calculates the next CRC value.
             *
             * @param crc Current CRC value
             *
             * @param data Byte to append to the crc value
             *
             * @returns New CRC value
             */
            inline uint8_t Calculate8(const uint8_t crc, const uint8_t data)
            {
                return detail::Calculate(detail::CrcTable8.data, crc, data);
            }
#endif

#ifdef CRC16
            /**
             * @brief Calculates the next CRC value.
             *
             * @param crc Current CRC value
             *
             * @param data Byte to append to the crc value
             *
             * @returns New CRC value
             */
            inline uint16_t Calculate16(const uint16_t crc, const uint8_t data)
            {
                return detail::Calculate(detail::CrcTable16.data, crc, data);
            }
#endif

#ifdef CRC32
            /**
             * @brief Calculates the next CRC value.
             *
             * @param crc Current CRC value
             *
             * @param data Byte to append to the crc value
             *
             * @returns New CRC value
             */
            inline uint32_t Calculate32(const uint32_t crc, const uint8_t data)
            {
                return detail::Calculate(detail::CrcTable32.data, crc, data);
            }
#endif
        }  // namespace crc
    }      // namespace shared
}  // namespace emb

#endif  // EMBMESSENGER_CRC_HPP