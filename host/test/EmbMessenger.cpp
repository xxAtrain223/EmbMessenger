#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "EmbMessenger/EmbMessenger.hpp"
#include "EmbMessenger/DataType.hpp"
#include "MockBuffer.hpp"

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
            MockBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            EXPECT_CALL(buffer, writeByte(0x01));
            EXPECT_CALL(buffer, writeByte(0x00));
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0xA7));

            auto pingCommand = std::make_shared<Ping>();
            messenger.send(pingCommand);

            EXPECT_CALL(buffer, update());
            EXPECT_CALL(buffer, messagesAvailable())
                .WillOnce(Return(1));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x01)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(DataType::kCrc)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(0x64)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2)); // readCrc
            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(false)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(false)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x01)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(0x01)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)); // readCrc -> nextCrc -> getType -> peekByte

            messenger.update();
        }

        TEST(messenger, set_led)
        {
            MockBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            EXPECT_CALL(buffer, writeByte(0x01)).Times(2);
            EXPECT_CALL(buffer, writeByte(DataType::kBoolTrue));
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0xCD));

            auto setLedCommandOn = std::make_shared<SetLed>(true);
            messenger.send(setLedCommandOn);

            EXPECT_CALL(buffer, update());
            EXPECT_CALL(buffer, messagesAvailable())
                .WillOnce(Return(1));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x01)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(DataType::kCrc)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(0x64)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2)); // readCrc
            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(false)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(false)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x01)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(0x01)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)); // readCrc -> nextCrc -> getType -> peekByte

            messenger.update();

            EXPECT_CALL(buffer, writeByte(0x02));
            EXPECT_CALL(buffer, writeByte(0x01));
            EXPECT_CALL(buffer, writeByte(DataType::kBoolFalse));
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0x3B));

            auto setLedCommandOff = std::make_shared<SetLed>(false);
            messenger.send(setLedCommandOff);

            EXPECT_CALL(buffer, update());
            EXPECT_CALL(buffer, messagesAvailable())
                .WillOnce(Return(1));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x02)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(DataType::kCrc)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(0xB0)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2)); // readCrc
            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(false)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(false)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x02)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(0x02)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)); // readCrc -> nextCrc -> getType -> peekByte

            messenger.update();
        }

        TEST(messenger, toggle_led)
        {
            MockBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            EXPECT_CALL(buffer, writeByte(0x01));
            EXPECT_CALL(buffer, writeByte(0x02));
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0x3F));

            auto toggleLedCommandOn = std::make_shared<ToggleLed>();
            messenger.send(toggleLedCommandOn);

            EXPECT_CALL(buffer, update());
            EXPECT_CALL(buffer, messagesAvailable())
                .WillOnce(Return(1));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x01)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(DataType::kBoolTrue)) // receive -> read(bool) -> readByte
                .WillOnce(Return(DataType::kCrc)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(0x50)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2)); // readCrc
            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(false)) // command->receive -> read -> readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // command->receive -> read -> read(bool) -> nextBool -> getType -> peekByte
                .WillOnce(Return(false)) // command->receive -> read -> read(bool) -> readByte
                .WillOnce(Return(false)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(false)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x01)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(0x01)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(DataType::kBoolTrue)) // command->receive -> read -> readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(DataType::kBoolTrue)) // command->receive -> read -> read(bool) -> nextBool -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)); // readCrc -> nextCrc -> getType -> peekByte

            messenger.update();

            EXPECT_CALL(buffer, writeByte(0x02)).Times(2);
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0xB3));

            auto toggleLedCommandOff = std::make_shared<ToggleLed>();
            messenger.send(toggleLedCommandOff);

            EXPECT_CALL(buffer, update());
            EXPECT_CALL(buffer, messagesAvailable())
                .WillOnce(Return(1));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x02)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(DataType::kBoolFalse)) // receive -> read(bool) -> readByte
                .WillOnce(Return(DataType::kCrc)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(0x90)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2)); // readCrc
            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(false)) // command->receive -> read -> readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // command->receive -> read -> read(bool) -> nextBool -> getType -> peekByte
                .WillOnce(Return(false)) // command->receive -> read -> read(bool) -> readByte
                .WillOnce(Return(false)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(false)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x02)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(0x02)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(DataType::kBoolFalse)) // command->receive -> read -> readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(DataType::kBoolFalse)) // command->receive -> read -> read(bool) -> nextBool -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)); // readCrc -> nextCrc -> getType -> peekByte

            messenger.update();
        }

        TEST(messenger, add)
        {
            MockBuffer buffer;

            EmbMessenger messenger(&buffer);
            messenger.registerCommand<Ping>(0);
            messenger.registerCommand<SetLed>(1);
            messenger.registerCommand<ToggleLed>(2);
            messenger.registerCommand<Add>(3);

            EXPECT_CALL(buffer, writeByte(0x01));
            EXPECT_CALL(buffer, writeByte(0x03));
            EXPECT_CALL(buffer, writeByte(0x07));
            EXPECT_CALL(buffer, writeByte(0x02));
            EXPECT_CALL(buffer, writeByte(DataType::kCrc));
            EXPECT_CALL(buffer, writeByte(0xEA));

            auto addCommand = std::make_shared<Add>(7, 2);
            messenger.send(addCommand);

            EXPECT_CALL(buffer, update());
            EXPECT_CALL(buffer, messagesAvailable())
                .WillOnce(Return(1));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x01)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(0x09)) // receive -> read(int32_t) -> readByte
                .WillOnce(Return(DataType::kCrc)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(0xB1)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2)); // readCrc
            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(false)) // read(uint8_t) -> getType -> readByte
                .WillOnce(Return(false)) // command->receive -> read -> readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // command->receive -> read -> read(int32_t) -> nextSignedInt -> getType -> peekByte
                .WillOnce(Return(false)) // command->receive -> read -> read(int16_t) -> nextSignedInt -> getType -> peekByte
                .WillOnce(Return(false)) // command->receive -> read -> read(int8_t) -> nextSignedInt -> getType -> peekByte
                .WillOnce(Return(false)) // command->receive -> read -> read(int8_t) -> readByte
                .WillOnce(Return(false)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(false)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> nextCrc -> getType -> peekByte
                .WillOnce(Return(false)) // readCrc -> removeByte -> readByte
                .WillOnce(Return(false)); // readCrc -> removeByte -> readByte
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x01)) // read(uint16_t) -> getType -> peekByte
                .WillOnce(Return(0x01)) // read(uint8_t) -> getType -> peekByte
                .WillOnce(Return(0x09)) // command->receive -> read -> readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(0x09)) // command->receive -> read -> read(int32_t) -> nextSignedInt -> getType -> peekByte
                .WillOnce(Return(0x09)) // command->receive -> read -> read(int16_t) -> nextSignedInt -> getType -> peekByte
                .WillOnce(Return(0x09)) // command->receive -> read -> read(int8_t) -> nextSignedInt -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // readErrors -> nextError -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)) // nextCrc -> getType -> peekByte
                .WillOnce(Return(DataType::kCrc)); // readCrc -> nextCrc -> getType -> peekByte

            messenger.update();
        }
    }
}