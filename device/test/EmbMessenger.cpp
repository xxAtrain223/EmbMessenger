#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "EmbMessenger/DataType.hpp"
#include "EmbMessenger/EmbMessenger.hpp"
#include "FakeBuffer.hpp"

using namespace testing;

namespace emb
{
    namespace device
    {
        namespace test
        {
            TEST(device_command, ping)
            {
                bool ledState = false;

                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};
                std::function<void()> setLed = [&] { messenger->read(ledState); };
                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };
                std::function<void()> add = [&] {
                    int a, b;
                    messenger->read(a, b);
                    messenger->write(a + b);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping, setLed, toggleLed, add };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x00 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(device_command, set_led)
            {
                bool ledState = false;

                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};
                std::function<void()> setLed = [&] { messenger->read(ledState); };
                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };
                std::function<void()> add = [&] {
                    int a, b;
                    messenger->read(a, b);
                    messenger->write(a + b);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping, setLed, toggleLed, add };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x01, shared::DataType::kBoolTrue });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
                ASSERT_EQ(ledState, true);

                buffer.addHostMessage({ 0x02, 0x01, shared::DataType::kBoolFalse });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02 }));
                ASSERT_EQ(ledState, false);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(device_command, toggle_led)
            {
                bool ledState = false;

                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};
                std::function<void()> setLed = [&] { messenger->read(ledState); };
                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };
                std::function<void()> add = [&] {
                    int a, b;
                    messenger->read(a, b);
                    messenger->write(a + b);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping, setLed, toggleLed, add };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x02 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, shared::DataType::kBoolTrue }));
                ASSERT_EQ(ledState, true);

                buffer.addHostMessage({ 0x02, 0x02 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, shared::DataType::kBoolFalse }));
                ASSERT_EQ(ledState, false);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(device_command, add)
            {
                bool ledState = false;

                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};
                std::function<void()> setLed = [&] { messenger->read(ledState); };
                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };
                std::function<void()> add = [&] {
                    int a, b;
                    messenger->read(a, b);
                    messenger->write(a + b);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping, setLed, toggleLed, add };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x03, 0x07, 0x02 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, 0x09 }));
                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_builtin_command, register_periodic_commands)
            {
                bool ledState = false;

                uint32_t millis_value = 0;

                std::shared_ptr<EmbMessenger<1>> messenger;

                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { toggleLed };
                messenger = std::make_shared<EmbMessenger<1>>(&buffer, commands, ARRAY_SIZE(commands),
                                                              [&]() { return millis_value; });

                buffer.addHostMessage(
                    { 0x01, shared::DataType::kUint16, 0xFF, 0xFE, 0x00, shared::DataType::kUint16, 0x03, 0xE8 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, shared::DataType::kBoolTrue }));
                ASSERT_TRUE(buffer.buffersEmpty());

                millis_value += 600;
                messenger->update();
                ASSERT_TRUE(buffer.buffersEmpty());

                millis_value += 600;
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, shared::DataType::kBoolFalse }));
                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_builtin_command, unregister_periodic_commands)
            {
                bool ledState = false;

                uint32_t millis_value = 0;

                std::shared_ptr<EmbMessenger<1>> messenger;

                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { toggleLed };
                messenger = std::make_shared<EmbMessenger<1>>(&buffer, commands, ARRAY_SIZE(commands),
                                                              [&]() { return millis_value; });

                buffer.addHostMessage(
                    { 0x01, shared::DataType::kUint16, 0xFF, 0xFE, 0x00, shared::DataType::kUint16, 0x03, 0xE8 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, shared::DataType::kBoolTrue }));
                ASSERT_TRUE(buffer.buffersEmpty());

                millis_value += 600;
                buffer.addHostMessage({ 0x02, shared::DataType::kUint16, 0xFF, 0xFD, 0x00 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02, 0x01 }));
                ASSERT_TRUE(buffer.buffersEmpty());

                millis_value += 600;
                messenger->update();
                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_builtin_command, reset_periodic_commands)
            {
                bool ledState = false;

                uint32_t millis_value = 0;

                std::shared_ptr<EmbMessenger<1>> messenger;

                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { toggleLed };
                messenger = std::make_shared<EmbMessenger<1>>(&buffer, commands, ARRAY_SIZE(commands),
                                                              [&]() { return millis_value; });

                buffer.addHostMessage(
                    { 0x01, shared::DataType::kUint16, 0xFF, 0xFE, 0x00, shared::DataType::kUint16, 0x03, 0xE8 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01 }));
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x01, shared::DataType::kBoolTrue }));
                ASSERT_TRUE(buffer.buffersEmpty());

                millis_value += 600;
                buffer.addHostMessage({ 0x02, shared::DataType::kUint16, 0xFF, 0xFF });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer({ 0x02 }));
                ASSERT_TRUE(buffer.buffersEmpty());

                millis_value += 600;
                messenger->update();
                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_errors, parameter_read_error)
            {
                bool ledState = false;

                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};
                std::function<void()> setLed = [&] { messenger->read(ledState); };
                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };
                std::function<void()> add = [&] {
                    int a, b;
                    messenger->read(a, b);
                    messenger->write(a + b);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping, setLed, toggleLed, add };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x03, shared::DataType::kBoolTrue });
                buffer.addHostMessage({ 0x02, 0x03, 0x07 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x01, shared::DataType::kError, shared::DataError::kParameterReadError, 0x00 }));
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x02, shared::DataType::kError, shared::DataError::kParameterReadError, 0x01 }));

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_errors, parameter_invalid)
            {
                bool ledState = false;

                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};
                std::function<void()> setLed = [&] { messenger->read(ledState); };
                std::function<void()> toggleLed = [&] {
                    ledState = !ledState;
                    messenger->write(ledState);
                };
                std::function<void()> add = [&] {
                    int a, b;
                    messenger->read(a, [](int val) { return val != 0; }, b, [](int val) { return val != 0; });
                    messenger->write(a + b);
                };

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping, setLed, toggleLed, add };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x03, 0x00, 0x07 });
                buffer.addHostMessage({ 0x02, 0x03, 0x07, 0x00 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x01, shared::DataType::kError, shared::DataError::kParameterInvalid, 0x00 }));
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x02, shared::DataType::kError, shared::DataError::kParameterInvalid, 0x01 }));

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_errors, message_id_read_error)
            {
                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({});
                buffer.addHostMessage({ shared::DataType::kBoolFalse });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { shared::DataType::kError, shared::DataError::kMessageIdReadError, 0x00 }));
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { shared::DataType::kError, shared::DataError::kMessageIdReadError, 0x00 }));

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_errors, command_id_read_error)
            {
                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01 });
                buffer.addHostMessage({ 0x02, shared::DataType::kBoolFalse });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x01, shared::DataType::kError, shared::DataError::kCommandIdReadError, 0x00 }));
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x02, shared::DataType::kError, shared::DataError::kCommandIdReadError, 0x00 }));

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_errors, command_id_invalid)
            {
                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x01 });
                buffer.addHostMessage({ 0x02, 0x02 });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x01, shared::DataType::kError, shared::DataError::kCommandIdInvalid, 0x01 }));
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x02, shared::DataType::kError, shared::DataError::kCommandIdInvalid, 0x02 }));

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_errors, crc_invalid)
            {
                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};

                FakeBuffer buffer;
                buffer.writeValidCrc(false);
                EmbMessenger<>::CommandFunction commands[] = { ping };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x00 });
                messenger->update();
                ASSERT_TRUE(
                    buffer.checkDeviceBuffer({ 0x01, shared::DataType::kError, shared::DataError::kCrcInvalid, 0x00 }));

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_errors, extra_parameters)
            {
                std::shared_ptr<EmbMessenger<>> messenger;

                std::function<void()> ping = [&] {};

                FakeBuffer buffer;
                EmbMessenger<>::CommandFunction commands[] = { ping };
                messenger = std::make_shared<EmbMessenger<>>(&buffer, commands, ARRAY_SIZE(commands));

                buffer.addHostMessage({ 0x01, 0x00, shared::DataType::kBoolFalse });
                messenger->update();
                ASSERT_TRUE(buffer.checkDeviceBuffer(
                    { 0x01, shared::DataType::kError, shared::DataError::kExtraParameters, 0x00 }));

                ASSERT_TRUE(buffer.buffersEmpty());
            }
        }  // namespace test
    }  // namespace device
}  // namespace emb
