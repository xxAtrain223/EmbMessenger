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

        TEST(messenger_builtin_command, register_periodic_commands)
        {
            FakeBuffer buffer;

            uint32_t millis_value = 0;

            EmbMessenger<1, 1> messenger(&buffer, [&]() { return millis_value; });

            bool ledState = false;

            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };

            messenger.attachCommand(0, toggleLed);

            buffer.addHostMessage({ 0x01, DataType::kUint8, 0xFE, 0x00, DataType::kUint16, 0x03, 0xE8 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kBoolTrue }));
            ASSERT_TRUE(buffer.buffersEmpty());

            millis_value += 600;
            messenger.update();
            ASSERT_TRUE(buffer.buffersEmpty());

            millis_value += 600;
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kBoolFalse }));
            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_builtin_command, unregister_periodic_commands)
        {
            FakeBuffer buffer;

            uint32_t millis_value = 0;

            EmbMessenger<1, 1> messenger(&buffer, [&]() { return millis_value; });

            bool ledState = false;

            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };

            messenger.attachCommand(0, toggleLed);

            buffer.addHostMessage({ 0x01, DataType::kUint8, 0xFE, 0x00, DataType::kUint16, 0x03, 0xE8 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kBoolTrue }));
            ASSERT_TRUE(buffer.buffersEmpty());

            millis_value += 600;
            buffer.addHostMessage({ 0x02, DataType::kUint8, 0xFD, 0x00 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, 0x01 }));
            ASSERT_TRUE(buffer.buffersEmpty());

            millis_value += 600;
            messenger.update();
            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_builtin_command, reset_periodic_commands)
        {
            FakeBuffer buffer;

            uint32_t millis_value = 0;

            EmbMessenger<1, 1> messenger(&buffer, [&]() { return millis_value; });

            bool ledState = false;

            std::function<void()> toggleLed = [&] {
                ledState = !ledState;
                messenger.write(ledState);
            };

            messenger.attachCommand(0, toggleLed);

            buffer.addHostMessage({ 0x01, DataType::kUint8, 0xFE, 0x00, DataType::kUint16, 0x03, 0xE8 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kBoolTrue }));
            ASSERT_TRUE(buffer.buffersEmpty());

            millis_value += 600;
            buffer.addHostMessage({ 0x02, DataType::kUint8, 0xFF });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02 }));
            ASSERT_TRUE(buffer.buffersEmpty());

            millis_value += 600;
            messenger.update();
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
            buffer.addHostMessage({ 0x02, 0x03, 0x07 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kParameterReadError, 0x00 }));
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, DataType::kError, DataError::kParameterReadError, 0x01 }));

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
            std::function<void()> add = [&] {
                int a = 0, b = 0;
                messenger.read_and_validate(
                    a, [](int val) { return val != 0; },
                    b, [](int val) { return val != 0; });
                messenger.write(a + b);
            };

            messenger.attachCommand(0, ping);
            messenger.attachCommand(1, setLed);
            messenger.attachCommand(2, toggleLed);
            messenger.attachCommand(3, add);

            buffer.addHostMessage({ 0x01, 0x03, 0x00, 0x07 });
            buffer.addHostMessage({ 0x02, 0x03, 0x07, 0x00 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kParameterInvalid, 0x00 }));
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, DataType::kError, DataError::kParameterInvalid, 0x01 }));

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_errors, message_id_read_error)
        {
            FakeBuffer buffer;

            EmbMessenger<1, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};

            messenger.attachCommand(0, ping);

            buffer.addHostMessage({ });
            buffer.addHostMessage({ DataType::kBoolFalse });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ DataType::kError, DataError::kMessageIdReadError, 0x00 }));
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ DataType::kError, DataError::kMessageIdReadError, 0x00 }));

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_errors, command_id_read_error)
        {
            FakeBuffer buffer;

            EmbMessenger<1, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};

            messenger.attachCommand(0, ping);

            buffer.addHostMessage({ 0x01 });
            buffer.addHostMessage({ 0x02, DataType::kBoolFalse });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kCommandIdReadError, 0x00 }));
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, DataType::kError, DataError::kCommandIdReadError, 0x00 }));

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_errors, command_id_invalid)
        {
            FakeBuffer buffer;

            EmbMessenger<1, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};

            messenger.attachCommand(0, ping);

            buffer.addHostMessage({ 0x01, 0x01 });
            buffer.addHostMessage({ 0x02, 0x02 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kCommandIdInvalid, 0x01 }));
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, DataType::kError, DataError::kCommandIdInvalid, 0x02 }));

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_errors, crc_invalid)
        {
            FakeBuffer buffer;
            buffer.writeValidCrc(false);

            EmbMessenger<2, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};

            messenger.attachCommand(0, ping);

            buffer.addHostMessage({ 0x01, 0x00 });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kCrcInvalid, 0x00 }));

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_errors, extra_parameters)
        {
            FakeBuffer buffer;

            EmbMessenger<1, 0> messenger(&buffer);

            bool ledState = false;

            std::function<void()> ping = [&] {};

            messenger.attachCommand(0, ping);

            buffer.addHostMessage({ 0x01, 0x00, DataType::kBoolFalse });
            messenger.update();
            ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, DataType::kError, DataError::kExtraParameters, 0x00 }));

            ASSERT_TRUE(buffer.buffersEmpty());
        }
    }
}
