#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "EmbMessenger/EmbMessenger.hpp"
#include "EmbMessenger/DataType.hpp"
#include "MockBuffer.hpp"
#include "Ping.hpp"
#include "SetLed.hpp"
#include "ToggleLed.hpp"

using namespace testing;

namespace emb
{
    namespace test
    {
        TEST(messenger, ping)
        {
            MockBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);

            EXPECT_CALL(buffer, writeByte(0x01));
            EXPECT_CALL(buffer, writeByte(0x00));
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0xA7));

            auto pingCommand = std::make_shared<Ping>();
            messenger.send(pingCommand);
        }

        TEST(messenger, set_led)
        {
            MockBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);

            EXPECT_CALL(buffer, writeByte(0x01)).Times(2);
            EXPECT_CALL(buffer, writeByte(DataType::kBoolTrue));
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0xCD));

            auto setLedCommandOn = std::make_shared<SetLed>(true);
            messenger.send(setLedCommandOn);

            EXPECT_CALL(buffer, writeByte(0x02));
            EXPECT_CALL(buffer, writeByte(0x01));
            EXPECT_CALL(buffer, writeByte(DataType::kBoolFalse));
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0x3B));

            auto setLedCommandOff = std::make_shared<SetLed>(false);
            messenger.send(setLedCommandOff);
        }
    }
}