#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "EmbMessenger/EmbMessenger.hpp"
#include "EmbMessenger/DataType.hpp"
#include "FakeBuffer.hpp"

#include "Ping.hpp"
#include "SetLed.hpp"
#include "ToggleLed.hpp"
#include "Add.hpp"

using namespace testing;

namespace emb
{
    namespace test
    {
        TEST(messenger, ping)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto pingCommand = std::make_shared<Ping>();
            messenger.send(pingCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
            buffer.addDeviceMessage({ 0x01 });

            messenger.update();

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger, set_led)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto setLedCommand = std::make_shared<SetLed>(true);
            messenger.send(setLedCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x01, DataType::kBoolTrue }));
            buffer.addDeviceMessage({ 0x01 });

            messenger.update();

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger, toggle_led)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto toggleLedCommand = std::make_shared<ToggleLed>();
            messenger.send(toggleLedCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x02 }));
            buffer.addDeviceMessage({ 0x01, DataType::kBoolTrue });

            messenger.update();

            ASSERT_TRUE(buffer.buffersEmpty());
            ASSERT_EQ(toggleLedCommand->ledState, true);
        }

        TEST(messenger, add)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto addCommand = std::make_shared<Add>(7, 2);
            messenger.send(addCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x03, 0x07, 0x02 }));
            buffer.addDeviceMessage({ 0x01, 0x09 });

            messenger.update();

            ASSERT_TRUE(buffer.buffersEmpty());
            ASSERT_EQ(addCommand->Result, 9);
        }

        TEST(messenger, reset)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            messenger.resetDevice();

            ASSERT_TRUE(buffer.checkHostBuffer({0x01, DataType::kUint8, 0xFF}));
            buffer.addDeviceMessage({0x01});

            messenger.update();

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger, register_periodic_command)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            bool ledState = false;
            messenger.registerPeriodicCommand<ToggleLed>(1000, [&](std::shared_ptr<ToggleLed>&& toggleLed) {
                ledState = toggleLed->ledState;
            });

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, DataType::kUint8, 0xFE, 0x02, DataType::kUint16, 0x03, 0xE8 }));

            buffer.addDeviceMessage({ 0x01 });
            buffer.addDeviceMessage({ 0x01, DataType::kBoolTrue });
            messenger.update();
            messenger.update();
            ASSERT_TRUE(buffer.buffersEmpty());
            ASSERT_EQ(ledState, true);

            buffer.addDeviceMessage({ 0x01, DataType::kBoolFalse });
            messenger.update();
            ASSERT_TRUE(buffer.buffersEmpty());
            ASSERT_EQ(ledState, false);
        }

        TEST(messenger, unregister_periodic_command)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            bool ledState = false;
            messenger.registerPeriodicCommand<ToggleLed>(1000, [&](std::shared_ptr<ToggleLed>&& toggleLed) {
                ledState = toggleLed->ledState;
            });

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, DataType::kUint8, 0xFE, 0x02, DataType::kUint16, 0x03, 0xE8 }));

            buffer.addDeviceMessage({ 0x01 });
            buffer.addDeviceMessage({ 0x01, DataType::kBoolTrue });
            messenger.update();
            messenger.update();
            ASSERT_TRUE(buffer.buffersEmpty());
            ASSERT_EQ(ledState, true);

            messenger.unregisterPeriodicCommand<ToggleLed>();

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x02, DataType::kUint8, 0xFD, 0x02 }));
            buffer.addDeviceMessage({ 0x02, 0x01 });
            messenger.update();
            ASSERT_TRUE(buffer.buffersEmpty());
        }
    }
}
