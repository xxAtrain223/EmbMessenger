#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "EmbMessenger/EmbMessenger.hpp"
#include "EmbMessenger/DataType.hpp"
#include "FakeBuffer.hpp"

using namespace testing;

namespace emb
{
    namespace test
    {
        TEST(messenger_command, ping)
        {
            FakeBuffer buffer;

            EmbMessenger<4, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};
            std::function<void()> setLed = [&] {
                messenger.read(ledState);
            };
            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };
            std::function<void()> add = [&] {
                int a, b;
                messenger.read(a, b);
                messenger.write(a + b);
            };

            messenger.attachCommand(0, ping);
            messenger.attachCommand(1, setLed);
            messenger.attachCommand(2, toggleLed);
            messenger.attachCommand(3, add);

            buffer.addHostMessage({ 0x01, 0x00 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_command, set_led)
        {
            FakeBuffer buffer;

            EmbMessenger<4, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};
            std::function<void()> setLed = [&] {
                messenger.read(ledState);
            };
            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };
            std::function<void()> add = [&] {
                int a, b;
                messenger.read(a, b);
                messenger.write(a + b);
            };

            messenger.attachCommand(0, ping);
            messenger.attachCommand(1, setLed);
            messenger.attachCommand(2, toggleLed);
            messenger.attachCommand(3, add);

            buffer.addHostMessage({ 0x01, 0x01, DataType::kBoolTrue });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
            ASSERT_EQ(ledState, true);

            buffer.addHostMessage({ 0x02, 0x01, DataType::kBoolFalse });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02 }));
            ASSERT_EQ(ledState, false);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_command, toggle_led)
        {
            FakeBuffer buffer;

            EmbMessenger<4, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};
            std::function<void()> setLed = [&] {
                messenger.read(ledState);
            };
            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };
            std::function<void()> add = [&] {
                int a, b;
                messenger.read(a, b);
                messenger.write(a + b);
            };

            messenger.attachCommand(0, ping);
            messenger.attachCommand(1, setLed);
            messenger.attachCommand(2, toggleLed);
            messenger.attachCommand(3, add);

            buffer.addHostMessage({ 0x01, 0x02 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kBoolTrue }));
            ASSERT_EQ(ledState, true);

            buffer.addHostMessage({ 0x02, 0x02 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, DataType::kBoolFalse }));
            ASSERT_EQ(ledState, false);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_command, add)
        {
            FakeBuffer buffer;

            EmbMessenger<4, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};
            std::function<void()> setLed = [&] {
                messenger.read(ledState);
            };
            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };
            std::function<void()> add = [&] {
                int a = 0, b = 0;
                messenger.read(a, b);
                messenger.write(a + b);
            };

            messenger.attachCommand(0, ping);
            messenger.attachCommand(1, setLed);
            messenger.attachCommand(2, toggleLed);
            messenger.attachCommand(3, add);

            buffer.addHostMessage({ 0x01, 0x03, 0x07, 0x02 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, 0x09 }));
            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_errors, parameter_read_error)
        {
            FakeBuffer buffer;

            EmbMessenger<4, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};
            std::function<void()> setLed = [&] {
                messenger.read(ledState);
            };
            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };
            std::function<void()> add = [&] {
                int a = 0, b = 0;
                messenger.read(a, b);
                messenger.write(a + b);
            };

            messenger.attachCommand(0, ping);
            messenger.attachCommand(1, setLed);
            messenger.attachCommand(2, toggleLed);
            messenger.attachCommand(3, add);

            buffer.addHostMessage({ 0x01, 0x03, DataType::kBoolTrue });
            buffer.addHostMessage({ 0x01, 0x03, 0x07 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kParameter0ReadError }));
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kParameter1ReadError }));

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_errors, parameter_invalid)
        {
            FakeBuffer buffer;

            EmbMessenger<4, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};
            std::function<void()> setLed = [&] {
                messenger.read(ledState);
            };
            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };
            std::function<bool(int)> validator = [](int val) { return val != 0; };
            std::function<void()> add = [&] {
                int a = 0, b = 0;
                messenger.read(a, validator, b, validator);
                messenger.write(a + b);
            };

            messenger.attachCommand(0, ping);
            messenger.attachCommand(1, setLed);
            messenger.attachCommand(2, toggleLed);
            messenger.attachCommand(3, add);

            buffer.addHostMessage({ 0x01, 0x03, 0x00, 0x07 });
            buffer.addHostMessage({ 0x02, 0x03, 0x07, 0x00 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kParameter0Invalid }));
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, DataType::kError, DataError::kParameter1Invalid }));

            ASSERT_TRUE(buffer.buffersEmpty());
        }
    }
}
