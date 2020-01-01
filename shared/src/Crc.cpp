#include "EmbMessenger/Crc.hpp"

namespace emb
{
    namespace shared
    {
        namespace crc
        {
            namespace detail
            {
#ifdef CRC8
                const PROGMEM CrcTable<uint8_t> CrcTable8 = compute<uint8_t, 0x1D>(0);
#endif
#ifdef CRC16
                const PROGMEM CrcTable<uint16_t> CrcTable16 = compute<uint16_t, 0x1021>(0);
#endif
#ifdef CRC32
                const PROGMEM CrcTable<uint32_t> CrcTable32 = compute<uint32_t, 0x04C11DB7>(0);
#endif
            }  // namespace detail
        }  // namespace crc
    }  // namespace shared
}  // namespace emb