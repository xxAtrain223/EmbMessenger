#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "EmbMessenger/EmbMessenger.hpp"
#include "EmbMessenger/DataType.hpp"
#include "FakeBuffer.hpp"

#include "Ping.hpp"
#include "SetLed.hpp"
#include "ToggleLed.hpp"
#include "Add.hpp"
#include "UserError.hpp"

using namespace testing;

namespace emb
{
    namespace test
    {
        TEST(messenger_command, ping)
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

        TEST(messenger_command, set_led)
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

        TEST(messenger_command, toggle_led)
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

        TEST(messenger_command, add)
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

        TEST(messenger_builtin_command, reset)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            messenger.resetDevice();

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, DataType::kUint8, 0xFF }));
            buffer.addDeviceMessage({ 0x01 });

            messenger.update();

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_builtin_command, register_periodic_command)
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

        TEST(messenger_builtin_command, unregister_periodic_command)
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

        TEST(messenger_exceptions_host, no_messages_available)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);

            ASSERT_NO_THROW(messenger.update());
        }

        TEST(messenger_exceptions_host, message_id_read_error)
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
            buffer.addDeviceMessage({ DataType::kBoolFalse });

            ASSERT_THROW(messenger.update(), MessageIdReadErrorHostException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_host, parameter_read_error)
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
            buffer.addDeviceMessage({ 0x01, 0x07 });

            ASSERT_THROW(messenger.update(), ParameterReadErrorHostException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_host, crc_invalid)
        {
            FakeBuffer buffer;
            buffer.writeValidCrc(false);

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto pingCommand = std::make_shared<Ping>();
            messenger.send(pingCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
            buffer.addDeviceMessage({ 0x01 });

            ASSERT_THROW(messenger.update(), CrcInvalidHostException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_host, message_id_invalid)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);

            buffer.addDeviceMessage({ 0x01 });

            ASSERT_THROW(messenger.update(), MessageIdInvalidHostException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_host, extra_parameters)
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
            buffer.addDeviceMessage({ 0x01, DataType::kBoolFalse, 0x42 });

            ASSERT_THROW(messenger.update(), ExtraParametersHostException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, message_id_read_error)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);

            buffer.addDeviceMessage({ DataType::kError, DataError::kMessageIdReadError });

            ASSERT_THROW(messenger.update(), MessageIdReadErrorDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, command_id_read_error)
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
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kCommandIdReadError });

            ASSERT_THROW(messenger.update(), CommandIdReadErrorDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, parameter_read_error)
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
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kParameter0ReadError });

            ASSERT_THROW(messenger.update(), ParameterReadErrorDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, parameter_invalid)
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
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kParameter0Invalid });

            ASSERT_THROW(messenger.update(), ParameterInvalidDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, extra_parameters)
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
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kExtraParameters });

            ASSERT_THROW(messenger.update(), ExtraParametersDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, crc_read_error)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto setLedCommand = std::make_shared<Ping>();
            messenger.send(setLedCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kCrcReadError });

            ASSERT_THROW(messenger.update(), CrcReadErrorDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, message_id_invalid)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto setLedCommand = std::make_shared<Ping>();
            messenger.send(setLedCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kMessageIdInvalid });

            ASSERT_THROW(messenger.update(), MessageIdInvalidDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, command_id_invalid)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto setLedCommand = std::make_shared<ToggleLed>();
            messenger.send(setLedCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x02 }));
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kCommandIdInvalid });

            ASSERT_THROW(messenger.update(), CommandIdInvalidDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, crc_invalid)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            auto setLedCommand = std::make_shared<Ping>();
            messenger.send(setLedCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kCrcInvalid });

            ASSERT_THROW(messenger.update(), CrcInvalidDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, out_of_periodic_command_slots)
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
            buffer.addDeviceMessage({ 0x01, DataType::kError, DataError::kOutOfPeriodicCommandSlots });

            ASSERT_THROW(messenger.update(), OutOfPeriodicCommandSlotsDeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, user_defined_error)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<UserError>(0);

            auto userErrorCommand = std::make_shared<UserError>();
            messenger.send(userErrorCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
            buffer.addDeviceMessage({ 0x01, DataType::kError, 0x42 });

            ASSERT_THROW(messenger.update(), UserErrorException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, user_defined_error_unexpected)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<UserError>(0);

            auto userErrorCommand = std::make_shared<UserError>();
            messenger.send(userErrorCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
            buffer.addDeviceMessage({ 0x01, DataType::kError, 0x43 });

            ASSERT_THROW(messenger.update(), DeviceException);

            ASSERT_TRUE(buffer.buffersEmpty());
        }

        TEST(messenger_exceptions_device, resend_command)
        {
            FakeBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<UserError>(0);

            auto userErrorCommand = std::make_shared<UserError>();
            messenger.send(userErrorCommand);

            ASSERT_TRUE(buffer.checkHostBuffer({ 0x01, 0x00 }));
            buffer.addDeviceMessage({ 0x01, DataType::kError, 0x42 });

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
    }
}
