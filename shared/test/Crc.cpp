#include "EmbMessenger/Crc.hpp"
#include <gtest/gtest.h>

namespace emb
{
    TEST(Crc8, Calculate)
    {
        uint8_t crc = 0;
        crc = crc::Calculate8(crc, 0x42);
        ASSERT_EQ(crc, 0x29);
    }

    TEST(Crc8, CalculateAppendCrc)
    {
        uint8_t crc = 0;
        crc = crc::Calculate8(crc, 0x42);
        crc = crc::Calculate8(crc, crc);
        ASSERT_EQ(crc, 0x00);
    }
}  // namespace emb