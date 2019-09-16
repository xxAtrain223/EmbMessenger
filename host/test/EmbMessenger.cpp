#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "EmbMessenger/DataType.hpp"
#include "EmbMessenger/EmbMessenger.hpp"
#include "FakeBuffer.hpp"

#include "Add.hpp"
#include "Ping.hpp"
#include "SetLed.hpp"
#include "ToggleLed.hpp"
#include "UserError.hpp"

using namespace testing;

namespace emb
{
    namespace host
    {
        namespace test
        {
            TEST(host_command, ping)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

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

            TEST(host_command, set_led)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto setLedCommand = std::make_shared<SetLed>(true);
                messenger.send(setLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x01, shared::DataType::kBoolTrue }));
                buffer.addDeviceMessage({ 0x01 });

                messenger.update();

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(host_command, toggle_led)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto toggleLedCommand = std::make_shared<ToggleLed>();
                messenger.send(toggleLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x02 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kBoolTrue });

                messenger.update();

                ASSERT_TRUE(buffer.buffersEmpty());
                ASSERT_EQ(toggleLedCommand->ledState, true);
            }

            TEST(host_command, add)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

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

            TEST(host_command, command_state)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto addCommand = std::make_shared<Add>(7, 7);
                ASSERT_EQ(addCommand->getCommandState(), CommandState::NotSent);
                messenger.send(addCommand);
                ASSERT_EQ(addCommand->getCommandState(), CommandState::Sent);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x03, 0x07, 0x07 }));
                buffer.addDeviceMessage({ 0x01, 0x0E });

                messenger.update();

                ASSERT_EQ(addCommand->getCommandState(), CommandState::Received);
                ASSERT_TRUE(buffer.buffersEmpty());
                ASSERT_EQ(addCommand->Result, 14);
            }

            TEST(messenger_builtin_command, reset)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                messenger.resetDevice();

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, shared::DataType::kUint16, 0xFF, 0xFF }));
                buffer.addDeviceMessage({ 0x01 });

                messenger.update();

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_builtin_command, register_periodic_command)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                bool ledState = false;
                messenger.registerPeriodicCommand<ToggleLed>(
                    1000, [&](std::shared_ptr<ToggleLed>&& toggleLed) { ledState = toggleLed->ledState; });

                ASSERT_TRUE(buffer.checkHostBuffer(
                    { 0x01, shared::DataType::kUint16, 0xFF, 0xFE, 0x02, shared::DataType::kUint16, 0x03, 0xE8 }));

                buffer.addDeviceMessage({ 0x01 });
                buffer.addDeviceMessage({ 0x01, shared::DataType::kBoolTrue });
                messenger.update();
                messenger.update();
                ASSERT_TRUE(buffer.buffersEmpty());
                ASSERT_EQ(ledState, true);

                buffer.addDeviceMessage({ 0x01, shared::DataType::kBoolFalse });
                messenger.update();
                ASSERT_TRUE(buffer.buffersEmpty());
                ASSERT_EQ(ledState, false);
            }

            TEST(messenger_builtin_command, unregister_periodic_command)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                bool ledState = false;
                messenger.registerPeriodicCommand<ToggleLed>(
                    1000, [&](std::shared_ptr<ToggleLed>&& toggleLed) { ledState = toggleLed->ledState; });

                ASSERT_TRUE(buffer.checkHostBuffer(
                    { 0x01, shared::DataType::kUint16, 0xFF, 0xFE, 0x02, shared::DataType::kUint16, 0x03, 0xE8 }));

                buffer.addDeviceMessage({ 0x01 });
                buffer.addDeviceMessage({ 0x01, shared::DataType::kBoolTrue });
                messenger.update();
                messenger.update();
                ASSERT_TRUE(buffer.buffersEmpty());
                ASSERT_EQ(ledState, true);

                messenger.unregisterPeriodicCommand<ToggleLed>();

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x02, shared::DataType::kUint16, 0xFF, 0xFD, 0x02 }));
                buffer.addDeviceMessage({ 0x02, 0x01 });
                messenger.update();
                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_host, initialization_timeout)
            {
                FakeBuffer buffer;

                ASSERT_THROW(EmbMessenger messenger(&buffer, std::chrono::milliseconds(20)),
                             InitializationError);
            }

            TEST(messenger_exceptions_host, no_messages_available)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                ASSERT_NO_THROW(messenger.update());
            }

            TEST(messenger_exceptions_host, message_id_read_error)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto pingCommand = std::make_shared<Ping>();
                messenger.send(pingCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
                buffer.addDeviceMessage({ shared::DataType::kBoolFalse });

                ASSERT_THROW(messenger.update(), MessageIdReadError);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_host, parameter_read_error)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto toggleLedCommand = std::make_shared<ToggleLed>();
                messenger.send(toggleLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x02 }));
                buffer.addDeviceMessage({ 0x01, 0x07 });

                ASSERT_THROW(messenger.update(), ParameterReadError);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_host, crc_invalid)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                buffer.writeValidCrc(false);

                auto pingCommand = std::make_shared<Ping>();
                messenger.send(pingCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
                buffer.addDeviceMessage({ 0x01 });

                ASSERT_THROW(messenger.update(), CrcInvalid);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_host, message_id_invalid)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                buffer.addDeviceMessage({ 0x01 });

                ASSERT_THROW(messenger.update(), MessageIdInvalid);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_host, extra_parameters)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto toggleLedCommand = std::make_shared<ToggleLed>();
                messenger.send(toggleLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x02 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kBoolFalse, 0x42 });

                ASSERT_THROW(messenger.update(), ExtraParameters);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, message_id_read_error)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                buffer.addDeviceMessage({ shared::DataType::kError, shared::DataError::kMessageIdReadError });

                ASSERT_THROW(messenger.update(), MessageIdReadError);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, command_id_read_error)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto pingCommand = std::make_shared<Ping>();
                messenger.send(pingCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, shared::DataError::kCommandIdReadError });

                ASSERT_THROW(messenger.update(), CommandIdReadError);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, parameter_read_error)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto setLedCommand = std::make_shared<SetLed>(true);
                messenger.send(setLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x01, shared::DataType::kBoolTrue }));
                buffer.addDeviceMessage(
                    { 0x01, shared::DataType::kError, shared::DataError::kParameterReadError, 0x00 });

                ASSERT_THROW(messenger.update(), ParameterReadError);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, parameter_invalid)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto setLedCommand = std::make_shared<SetLed>(true);
                messenger.send(setLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x01, shared::DataType::kBoolTrue }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, shared::DataError::kParameterInvalid, 0x00 });

                ASSERT_THROW(messenger.update(), ParameterInvalid);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, extra_parameters)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto setLedCommand = std::make_shared<SetLed>(true);
                messenger.send(setLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x01, shared::DataType::kBoolTrue }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, shared::DataError::kExtraParameters });

                ASSERT_THROW(messenger.update(), ExtraParameters);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, crc_read_error)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto setLedCommand = std::make_shared<Ping>();
                messenger.send(setLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, shared::DataError::kCrcReadError });

                ASSERT_THROW(messenger.update(), CrcReadError);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, command_id_invalid)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto setLedCommand = std::make_shared<ToggleLed>();
                messenger.send(setLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x02 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, shared::DataError::kCommandIdInvalid });

                ASSERT_THROW(messenger.update(), CommandIdInvalid);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, crc_invalid)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                auto setLedCommand = std::make_shared<Ping>();
                messenger.send(setLedCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, shared::DataError::kCrcInvalid });

                ASSERT_THROW(messenger.update(), CrcInvalid);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, out_of_periodic_command_slots)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<Ping>(0);
                messenger.registerCommand<SetLed>(1);
                messenger.registerCommand<ToggleLed>(2);
                messenger.registerCommand<Add>(3);

                bool ledState = false;
                messenger.registerPeriodicCommand<ToggleLed>(
                    1000, [&](std::shared_ptr<ToggleLed>&& toggleLed) { ledState = toggleLed->ledState; });

                ASSERT_TRUE(buffer.checkHostBuffer(
                    { 0x01, shared::DataType::kUint16, 0xFF, 0xFE, 0x02, shared::DataType::kUint16, 0x03, 0xE8 }));
                buffer.addDeviceMessage(
                    { 0x01, shared::DataType::kError, shared::DataError::kOutOfPeriodicCommandSlots });

                ASSERT_THROW(messenger.update(), OutOfPeriodicCommandSlots);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, user_defined_error)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<UserError>(0);

                auto userErrorCommand = std::make_shared<UserError>();
                messenger.send(userErrorCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, 0x42 });

                ASSERT_THROW(messenger.update(), UserErrorException);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, user_defined_error_unexpected)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<UserError>(0);

                auto userErrorCommand = std::make_shared<UserError>();
                messenger.send(userErrorCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, 0x43 });

                ASSERT_THROW(messenger.update(), BaseException);

                ASSERT_TRUE(buffer.buffersEmpty());
            }

            TEST(messenger_exceptions_device, resend_command)
            {
                FakeBuffer buffer;

                buffer.addDeviceMessage({ 0x00 });
                EmbMessenger messenger(&buffer, std::chrono::seconds(1));
                ASSERT_TRUE(buffer.checkHostBuffer({ 0x00, shared::DataType::kUint16, 0xFF, 0xFF }));

                messenger.registerCommand<UserError>(0);

                auto userErrorCommand = std::make_shared<UserError>();
                messenger.send(userErrorCommand);

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
                buffer.addDeviceMessage({ 0x01, shared::DataType::kError, 0x42 });

                try
                {
                    messenger.update();
                }
                catch (UserErrorException e)
                {
                    ASSERT_TRUE(buffer.buffersEmpty());
                    ASSERT_EQ(e.getCommand(), userErrorCommand);
                    messenger.send(e.getCommand());
                }

                ASSERT_TRUE(buffer.checkHostBuffer({ 0x02, 0x00 }));
                buffer.addDeviceMessage({ 0x02 });

                messenger.update();
                ASSERT_TRUE(buffer.buffersEmpty());
            }
        }  // namespace test
    }  // namespace host
}  // namespace emb
