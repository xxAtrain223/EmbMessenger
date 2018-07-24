#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "EmbeddedMessenger/Reader.hpp"
#include "EmbeddedMessenger/DataType.hpp"
#include "EmbeddedMessenger/DataError.hpp"
#include "MockBuffer.hpp"

using namespace testing;

namespace emb
{
    namespace test
    {
        TEST(reader_types, get_type_none)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0xD8));

            Reader reader(&buffer);
            DataType type;

            EXPECT_FALSE(reader.getType(type));
            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_null)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kNull))
                .WillOnce(Return(DataType::kBoolFalse));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kNull);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kNull);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_bool_false)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kBoolFalse))
                .WillOnce(Return(DataType::kBoolTrue));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kBoolFalse);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kBoolFalse);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_bool_true)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kBoolTrue))
                .WillOnce(Return(DataType::kBoolFalse));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kBoolTrue);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kBoolTrue);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_uint_8)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint8))
                .WillOnce(Return(DataType::kUint16));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kUint8);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kUint8);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_uint_16)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint16))
                .WillOnce(Return(DataType::kUint32));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kUint16);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kUint16);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_uint_32)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint32))
                .WillOnce(Return(DataType::kUint64));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kUint32);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kUint32);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_uint_64)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint64))
                .WillOnce(Return(DataType::kUint32));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kUint64);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kUint64);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_int_8)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt8))
                .WillOnce(Return(DataType::kInt16));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kInt8);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kInt8);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_int_16)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt16))
                .WillOnce(Return(DataType::kInt32));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kInt16);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kInt16);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_int_32)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt32))
                .WillOnce(Return(DataType::kInt64));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kInt32);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kInt32);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_int_64)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt64))
                .WillOnce(Return(DataType::kInt32));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kInt64);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kInt64);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_posfixint)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x00))
                .WillOnce(Return(0x41))
                .WillOnce(Return(0x7F));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kPosFixInt);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kPosFixInt);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kPosFixInt);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_negfixint)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0xE0))
                .WillOnce(Return(0xF0))
                .WillOnce(Return(0xFF));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kNegFixInt);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kNegFixInt);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kNegFixInt);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, get_type_float)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kFloat))
                .WillOnce(Return(DataType::kUint8));

            Reader reader(&buffer);
            DataType type;

            EXPECT_TRUE(reader.getType(type));
            EXPECT_EQ(type, DataType::kFloat);

            EXPECT_TRUE(reader.getType(type));
            EXPECT_NE(type, DataType::kFloat);

            EXPECT_FALSE(reader.getType(type));
        }

        TEST(reader_types, next_null)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kNull))
                .WillOnce(Return(DataType::kCrc));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.nextNull());
            EXPECT_FALSE(reader.nextNull());
            EXPECT_FALSE(reader.nextNull());
        }

        TEST(reader_types, next_bool)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kBoolFalse))
                .WillOnce(Return(DataType::kBoolTrue))
                .WillOnce(Return(DataType::kNull))
                .WillOnce(Return(DataType::kCrc));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.nextBool());
            EXPECT_TRUE(reader.nextBool());
            EXPECT_FALSE(reader.nextBool());
            EXPECT_FALSE(reader.nextBool());
            EXPECT_FALSE(reader.nextBool());
        }

        TEST(reader_types, next_char)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x61))
                .WillOnce(Return(0x62))
                .WillOnce(Return(0xC0));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.nextChar());
            EXPECT_TRUE(reader.nextChar());
            EXPECT_FALSE(reader.nextChar());
            EXPECT_FALSE(reader.nextChar());
        }

        TEST(reader_types, next_unsigned_int)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint8))
                .WillOnce(Return(DataType::kUint16))
                .WillOnce(Return(DataType::kUint32))
                .WillOnce(Return(DataType::kUint64))
                .WillOnce(Return(DataType::kPosFixInt))
                .WillOnce(Return(DataType::kNegFixInt));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.nextUnsignedInt());
            EXPECT_TRUE(reader.nextUnsignedInt());
            EXPECT_TRUE(reader.nextUnsignedInt());
            EXPECT_TRUE(reader.nextUnsignedInt());
            EXPECT_TRUE(reader.nextUnsignedInt());
            EXPECT_FALSE(reader.nextUnsignedInt());
            EXPECT_FALSE(reader.nextUnsignedInt());
        }

        TEST(reader_types, next_signed_int)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt8))
                .WillOnce(Return(DataType::kInt16))
                .WillOnce(Return(DataType::kInt32))
                .WillOnce(Return(DataType::kInt64))
                .WillOnce(Return(DataType::kPosFixInt))
                .WillOnce(Return(DataType::kNegFixInt))
                .WillOnce(Return(DataType::kError));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.nextSignedInt());
            EXPECT_TRUE(reader.nextSignedInt());
            EXPECT_TRUE(reader.nextSignedInt());
            EXPECT_TRUE(reader.nextSignedInt());
            EXPECT_TRUE(reader.nextSignedInt());
            EXPECT_TRUE(reader.nextSignedInt());
            EXPECT_FALSE(reader.nextSignedInt());
            EXPECT_FALSE(reader.nextSignedInt());
        }

        TEST(reader_types, next_float)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kFloat))
                .WillOnce(Return(DataType::kNull));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.nextFloat());
            EXPECT_FALSE(reader.nextFloat());
            EXPECT_FALSE(reader.nextFloat());
        }

        TEST(reader_types, next_error)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kError))
                .WillOnce(Return(DataType::kCrc));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.nextError());
            EXPECT_FALSE(reader.nextError());
            EXPECT_FALSE(reader.nextError());
        }

        TEST(reader_types, next_crc)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kCrc))
                .WillOnce(Return(DataType::kError));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.nextCrc());
            EXPECT_FALSE(reader.nextCrc());
            EXPECT_FALSE(reader.nextCrc());
        }

        TEST(reader_readers, read_bool_false)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kBoolFalse))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kBoolFalse));

            Reader reader(&buffer);

            bool value = true;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, false);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_bool_true)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kBoolTrue))
                .WillOnce(Return(DataType::kCrc));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kBoolTrue));

            Reader reader(&buffer);

            bool value = false;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, true);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_uint8)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint8))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kUint8))
                .WillOnce(Return(0xF1));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2))
                .WillOnce(Return(1));

            Reader reader(&buffer);

            uint8_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, 0xF1);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_uint16)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint16))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kUint16))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(3))
                .WillOnce(Return(2));

            Reader reader(&buffer);

            uint16_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, 0xF123);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_uint32)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint32))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kUint32))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23))
                .WillOnce(Return(0x45))
                .WillOnce(Return(0x67));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(5))
                .WillOnce(Return(4));

            Reader reader(&buffer);

            uint32_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, 0xF1234567);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_uint64)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint64))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kUint64))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23))
                .WillOnce(Return(0x45))
                .WillOnce(Return(0x67))
                .WillOnce(Return(0x89))
                .WillOnce(Return(0xAB))
                .WillOnce(Return(0xCD))
                .WillOnce(Return(0xEF));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(9))
                .WillOnce(Return(8));

            Reader reader(&buffer);

            uint64_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, 0xF123456789ABCDEF);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_int8)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt8))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt8))
                .WillOnce(Return(0xDF));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2))
                .WillOnce(Return(1));

            Reader reader(&buffer);

            int8_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int8_t)0xDF);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_int16)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt16))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt16))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(3))
                .WillOnce(Return(2));

            Reader reader(&buffer);

            int16_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int16_t)0xF123);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_int32)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt32))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt32))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23))
                .WillOnce(Return(0x45))
                .WillOnce(Return(0x67));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(5))
                .WillOnce(Return(4));

            Reader reader(&buffer);

            int32_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int32_t)0xF1234567);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_int64)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt64))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt64))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23))
                .WillOnce(Return(0x45))
                .WillOnce(Return(0x67))
                .WillOnce(Return(0x89))
                .WillOnce(Return(0xAB))
                .WillOnce(Return(0xCD))
                .WillOnce(Return(0xEF));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(9))
                .WillOnce(Return(8));

            Reader reader(&buffer);

            int64_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int64_t)0xF123456789ABCDEF);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_posfixint_uint8)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x0F))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x0F));

            Reader reader(&buffer);

            uint8_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, 0x0F);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_posfixint_int8)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x0F))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x0F));

            Reader reader(&buffer);

            int8_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, 0x0F);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_negfixint_int8)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0xF0))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0xF0));

            Reader reader(&buffer);

            int8_t value;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int8_t)0xF0);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_float)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kFloat))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kFloat))
                .WillOnce(Return(0x40))
                .WillOnce(Return(0x49))
                .WillOnce(Return(0x0F))
                .WillOnce(Return(0xD0));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(5))
                .WillOnce(Return(4));

            Reader reader(&buffer);

            float value = 0.f;
            EXPECT_TRUE(reader.read(value));
            EXPECT_FLOAT_EQ(value, 3.14159f);
            EXPECT_FALSE(reader.read(value));
            EXPECT_FALSE(reader.read(value));
        }

        TEST(reader_readers, read_null)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kNull))
                .WillOnce(Return(DataType::kBoolFalse));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kNull));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.readNull());
            EXPECT_FALSE(reader.readNull());
            EXPECT_FALSE(reader.readNull());
        }

        TEST(reader_readers, read_error)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false)) // 1
                .WillOnce(Return(false)) // 1
                .WillOnce(Return(false)) // 2
                .WillOnce(Return(false)) // 3
                .WillOnce(Return(true)); // 4
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kError)) // 1
                .WillOnce(Return(DataType::kError)) // 3
                .WillOnce(Return(DataType::kNull)); // 2
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kError)) // 1
                .WillOnce(Return(DataError::kParameter0ReadError)); // 1
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2))  // 1
                .WillOnce(Return(2))  // 1
                .WillOnce(Return(1)); // 3

            Reader reader(&buffer);

            DataError value = DataError::kExtraParameters;
            EXPECT_TRUE(reader.readError(value));  // 1 Pass
            EXPECT_EQ(value, DataError::kParameter0ReadError);
            EXPECT_FALSE(reader.readError(value)); // 2 Fail - Type
            EXPECT_FALSE(reader.readError(value)); // 3 Fail - Size
            EXPECT_FALSE(reader.readError(value)); // 4 Fail - Empty
        }

        TEST(reader_readers, read_crc)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(false))
                .WillOnce(Return(true));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kCrc))
                .WillOnce(Return(DataType::kCrc))
                .WillOnce(Return(DataType::kCrc))
                .WillOnce(Return(DataType::kNull));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kCrc))
                .WillOnce(Return(0x28))
                .WillOnce(Return(DataType::kCrc))
                .WillOnce(Return(0x29));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2))
                .WillOnce(Return(2))
                .WillOnce(Return(1));

            Reader reader(&buffer);

            EXPECT_TRUE(reader.readCrc());
            EXPECT_FALSE(reader.readCrc());
            reader.resetCrc();
            EXPECT_FALSE(reader.readCrc());
            EXPECT_FALSE(reader.readCrc());
            EXPECT_FALSE(reader.readCrc());
        }

        TEST(reader_expand, uint8_to_uint16)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint8))
                .WillOnce(Return(DataType::kUint8));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kUint8))
                .WillOnce(Return(0xF1));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2))
                .WillOnce(Return(1));

            Reader reader(&buffer);

            uint16_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (uint16_t)0x00F1);
        }

        TEST(reader_expand, uint16_to_uint32)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint16))
                .WillOnce(Return(DataType::kUint16));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kUint16))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(3))
                .WillOnce(Return(2));

            Reader reader(&buffer);

            uint32_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (uint32_t)0x0000F123);
        }

        TEST(reader_expand, uint32_to_uint64)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kUint32))
                .WillOnce(Return(DataType::kUint32));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kUint32))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23))
                .WillOnce(Return(0x45))
                .WillOnce(Return(0x67));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(5))
                .WillOnce(Return(4));

            Reader reader(&buffer);

            uint64_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (uint64_t)0x00000000F1234567);
        }

        TEST(reader_expand, int8_to_int16_pos)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x71))
                .WillOnce(Return(0x71));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x71));

            Reader reader(&buffer);

            int16_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int16_t)0x0071);
        }

        TEST(reader_expand, int16_to_int32_pos)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt16))
                .WillOnce(Return(DataType::kInt16));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt16))
                .WillOnce(Return(0x71))
                .WillOnce(Return(0x23));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(3))
                .WillOnce(Return(2));

            Reader reader(&buffer);

            int32_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int32_t)0x00007123);
        }

        TEST(reader_expand, int32_to_int64_pos)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt32))
                .WillOnce(Return(DataType::kInt32));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt32))
                .WillOnce(Return(0x71))
                .WillOnce(Return(0x23))
                .WillOnce(Return(0x45))
                .WillOnce(Return(0x67));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(5))
                .WillOnce(Return(4));

            Reader reader(&buffer);

            int64_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int64_t)0x0000000071234567);
        }

        TEST(reader_expand, int8_to_int16_neg)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt8))
                .WillOnce(Return(DataType::kInt8));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt8))
                .WillOnce(Return(0x81));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(2))
                .WillOnce(Return(1));

            Reader reader(&buffer);

            int16_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int16_t)0xFF81);
        }

        TEST(reader_expand, int16_to_int32_neg)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt16))
                .WillOnce(Return(DataType::kInt16));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt16))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(3))
                .WillOnce(Return(2));

            Reader reader(&buffer);

            int32_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int32_t)0xFFFFF123);
        }

        TEST(reader_expand, int32_to_int64_neg)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(DataType::kInt32))
                .WillOnce(Return(DataType::kInt32));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(DataType::kInt32))
                .WillOnce(Return(0xF1))
                .WillOnce(Return(0x23))
                .WillOnce(Return(0x45))
                .WillOnce(Return(0x67));
            EXPECT_CALL(buffer, size())
                .WillOnce(Return(5))
                .WillOnce(Return(4));

            Reader reader(&buffer);

            int64_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (int64_t)0xFFFFFFFFF1234567);
        }

        TEST(reader_expand, posfixint_to_uint64)
        {
            MockBuffer buffer;

            EXPECT_CALL(buffer, empty())
                .WillRepeatedly(Return(false));
            EXPECT_CALL(buffer, peek())
                .WillOnce(Return(0x41))
                .WillOnce(Return(0x41))
                .WillOnce(Return(0x41))
                .WillOnce(Return(0x41));
            EXPECT_CALL(buffer, readByte())
                .WillOnce(Return(0x41));

            Reader reader(&buffer);

            uint64_t value = 0;
            EXPECT_TRUE(reader.read(value));
            EXPECT_EQ(value, (uint64_t)0x0000000000000041);
        }
    }
}