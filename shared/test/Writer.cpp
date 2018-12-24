#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "EmbMessenger/DataError.hpp"
#include "EmbMessenger/DataType.hpp"
#include "EmbMessenger/Writer.hpp"
#include "MockBuffer.hpp"

namespace emb
{
    namespace shared
    {
        namespace test
        {
            TEST(writer, write_false)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kBoolFalse));

                Writer writer(&buffer);

                writer.write(false);
            }

            TEST(writer, write_true)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kBoolTrue));

                Writer writer(&buffer);

                writer.write(true);
            }

            TEST(writer, write_uint8)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kUint8));
                EXPECT_CALL(buffer, writeByte(0xF1));

                Writer writer(&buffer);

                uint8_t data = 0xF1;
                writer.write(data);
            }

            TEST(writer, write_uint16)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kUint16));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));

                Writer writer(&buffer);

                uint16_t data = 0xF123;
                writer.write(data);
            }

            TEST(writer, write_uint32)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kUint32));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));
                EXPECT_CALL(buffer, writeByte(0x45));
                EXPECT_CALL(buffer, writeByte(0x67));

                Writer writer(&buffer);

                uint32_t data = 0xF1234567;
                writer.write(data);
            }

            TEST(writer, write_uint64)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kUint64));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));
                EXPECT_CALL(buffer, writeByte(0x45));
                EXPECT_CALL(buffer, writeByte(0x67));
                EXPECT_CALL(buffer, writeByte(0x89));
                EXPECT_CALL(buffer, writeByte(0xAB));
                EXPECT_CALL(buffer, writeByte(0xCD));
                EXPECT_CALL(buffer, writeByte(0xEF));

                Writer writer(&buffer);

                uint64_t data = 0xF123456789ABCDEF;
                writer.write(data);
            }

            TEST(writer, write_int8)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt8));
                EXPECT_CALL(buffer, writeByte(0xDF));

                Writer writer(&buffer);

                int8_t data = (int8_t)0xDF;
                writer.write(data);
            }

            TEST(writer, write_int16)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt16));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));

                Writer writer(&buffer);

                int16_t data = (int16_t)0xF123;
                writer.write(data);
            }

            TEST(writer, write_int32)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt32));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));
                EXPECT_CALL(buffer, writeByte(0x45));
                EXPECT_CALL(buffer, writeByte(0x67));

                Writer writer(&buffer);

                int32_t data = 0xF1234567;
                writer.write(data);
            }

            TEST(writer, write_int64)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt64));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));
                EXPECT_CALL(buffer, writeByte(0x45));
                EXPECT_CALL(buffer, writeByte(0x67));
                EXPECT_CALL(buffer, writeByte(0x89));
                EXPECT_CALL(buffer, writeByte(0xAB));
                EXPECT_CALL(buffer, writeByte(0xCD));
                EXPECT_CALL(buffer, writeByte(0xEF));

                Writer writer(&buffer);

                int64_t data = 0xF123456789ABCDEF;
                writer.write(data);
            }

            TEST(writer, write_float)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kFloat));
                EXPECT_CALL(buffer, writeByte(0x40));
                EXPECT_CALL(buffer, writeByte(0x49));
                EXPECT_CALL(buffer, writeByte(0x0F));
                EXPECT_CALL(buffer, writeByte(0xD0));

                Writer writer(&buffer);

                float data = 3.14159f;
                writer.write(data);
            }

            TEST(writer, write_null)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kNull));

                Writer writer(&buffer);

                writer.writeNull();
            }

            TEST(writer, write_error)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kError));
                EXPECT_CALL(buffer, writeByte(DataError::kParameterReadError));

                Writer writer(&buffer);

                writer.writeError(DataError::kParameterReadError);
            }

            TEST(writer, write_crc)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kCrc));
                EXPECT_CALL(buffer, writeByte(0x28));

                Writer writer(&buffer);

                writer.writeCrc();
            }

            TEST(writer_minimize, uint64_to_uint32)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kUint32));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));
                EXPECT_CALL(buffer, writeByte(0x45));
                EXPECT_CALL(buffer, writeByte(0x67));

                Writer writer(&buffer);

                uint64_t data = 0x00000000F1234567;
                writer.write(data);
            }

            TEST(writer_minimize, uint32_to_uint16)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kUint16));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));

                Writer writer(&buffer);

                uint32_t data = 0x0000F123;
                writer.write(data);
            }

            TEST(writer_minimize, uint16_to_uint8)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kUint8));
                EXPECT_CALL(buffer, writeByte(0xF1));

                Writer writer(&buffer);

                uint16_t data = 0x00F1;
                writer.write(data);
            }

            TEST(writer_minimize, uint8_to_posfixint)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(0x0F));

                Writer writer(&buffer);

                uint8_t data = 0x0F;
                writer.write(data);
            }

            TEST(writer_minimize, int64_to_int32_pos)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt32));
                EXPECT_CALL(buffer, writeByte(0x71));
                EXPECT_CALL(buffer, writeByte(0x23));
                EXPECT_CALL(buffer, writeByte(0x45));
                EXPECT_CALL(buffer, writeByte(0x67));

                Writer writer(&buffer);

                int64_t data = 0x0000000071234567;
                writer.write(data);
            }

            TEST(writer_minimize, int32_to_int16_pos)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt16));
                EXPECT_CALL(buffer, writeByte(0x71));
                EXPECT_CALL(buffer, writeByte(0x23));

                Writer writer(&buffer);

                int32_t data = 0x00007123;
                writer.write(data);
            }

            TEST(writer_minimize, int16_to_int8_pos)
            {
                // This will be converted into a Positive Fixed Int
                // A 1 byte signed int has a max positive value of 0x7F
                // Positive Fixed Ints are defined by range 0x00 - 0x7F

                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(0x71));

                Writer writer(&buffer);

                int16_t data = 0x0071;
                writer.write(data);
            }

            TEST(writer_minimize, int64_to_int32_neg)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt32));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));
                EXPECT_CALL(buffer, writeByte(0x45));
                EXPECT_CALL(buffer, writeByte(0x67));

                Writer writer(&buffer);

                int64_t data = 0xFFFFFFFFF1234567;
                writer.write(data);
            }

            TEST(writer_minimize, int32_to_int16_neg)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt16));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));

                Writer writer(&buffer);

                int32_t data = 0xFFFFF123;
                writer.write(data);
            }

            TEST(writer_minimize, int16_to_int8_neg)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt8));
                EXPECT_CALL(buffer, writeByte(0x81));

                Writer writer(&buffer);

                int16_t data = (int16_t)0xFF81;
                writer.write(data);
            }

            TEST(writer_minimize, int8_to_posfixint)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(0x0F));

                Writer writer(&buffer);

                int8_t data = 0x0F;
                writer.write(data);
            }

            TEST(writer_minimize, int8_to_negfixint)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(0xF0));

                Writer writer(&buffer);

                int8_t data = (int8_t)0xF0;
                writer.write(data);
            }

            TEST(writer_multiple_arguments, bool_uint8)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kBoolTrue));
                EXPECT_CALL(buffer, writeByte(DataType::kUint8));
                EXPECT_CALL(buffer, writeByte(0x88));

                Writer writer(&buffer);

                bool data1 = true;
                uint8_t data2 = 0x88;
                writer.write(data1);
                writer.write(data2);
            }

            TEST(writer_multiple_arguments, int16_float)
            {
                MockBuffer buffer;

                EXPECT_CALL(buffer, writeByte(DataType::kInt16));
                EXPECT_CALL(buffer, writeByte(0xF1));
                EXPECT_CALL(buffer, writeByte(0x23));
                EXPECT_CALL(buffer, writeByte(DataType::kFloat));
                EXPECT_CALL(buffer, writeByte(0x40));
                EXPECT_CALL(buffer, writeByte(0x49));
                EXPECT_CALL(buffer, writeByte(0x0F));
                EXPECT_CALL(buffer, writeByte(0xD0));

                Writer writer(&buffer);

                int16_t data1 = (int16_t)0xF123;
                float data2 = 3.14159f;
                writer.write(data1);
                writer.write(data2);
            }
        }  // namespace test
    }  // namespace shared
}  // namespace emb
