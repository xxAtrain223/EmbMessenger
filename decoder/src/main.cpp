#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <EmbMessenger/DataType.hpp>
#include <EmbMessenger/IBuffer.hpp>
#include <EmbMessenger/Reader.hpp>
#include <cstdio>

class DecodeBuffer : public emb::shared::IBuffer
{
    std::vector<uint8_t> buf;
    bool receivedEOM = false, readEOM = false;
    uint8_t messageCount = 0;

public:
    virtual void writeByte(const uint8_t byte) override
    {
        buf.emplace_back(byte);
        if (byte == emb::shared::kEndOfMessage)
        {
            receivedEOM = true;
        }
        else if (receivedEOM)
        {
            messageCount++;
            receivedEOM = false;
        }
    }

    virtual uint8_t peek() const override
    {
        return buf.at(0);
    }

    virtual uint8_t readByte() override
    {
        uint8_t byte = buf.at(0);
        buf.erase(buf.begin());
        if (byte == emb::shared::kEndOfMessage)
        {
            readEOM = true;
        }
        else if (readEOM)
        {
            messageCount--;
            readEOM = false;
        }

        return byte;
    }

    virtual bool empty() const override
    {
        return buf.empty();
    }

    virtual size_t size() const override
    {
        return buf.size();
    }

    virtual uint8_t messages() const override
    {
        return messageCount;
    }

    virtual void update() override {}

    virtual void zero() override
    {
        buf.clear();
        receivedEOM = false;
        messageCount = 0;
    }
};

void decode(DecodeBuffer& buffer)
{
    using namespace emb::shared;
    Reader reader(&buffer);
    uint16_t param = 0;

    emb::shared::DataType type;
    while (reader.getType(type))
    {
        printf("%2u: ", param);
        switch (type)
        {
        case kNull: {
            reader.readNull();
            printf("Null");
        } break;

        case kBoolFalse:
        case kBoolTrue: {
            bool value = false;
            reader.read(value);
            printf("%s", (value) ? "True" : "False");
        } break;

        case kPosFixInt:
        case kUint8: {
            uint8_t value;
            reader.read(value);
            printf("%u 0x%0*X", value, sizeof(value) * 2, value);
        } break;
        case kUint16: {
            uint16_t value;
            reader.read(value);
            printf("%u 0x%0*X", value, sizeof(value) * 2, value);
        } break;
        case kUint32: {
            uint32_t value;
            reader.read(value);
            printf("%u 0x%0*X", value, sizeof(value) * 2, value);
        } break;
        case kUint64: {
            uint64_t value;
            reader.read(value);
            printf("%u 0x%0*X", value, sizeof(value) * 2, value);
        } break;

        case kNegFixInt:
        case kInt8:
        case kInt16:
        case kInt32:
        case kInt64: {
            int64_t value;
            reader.read(value);
            printf("%d", value);
        } break;

        case kFloat: {
            float value;
            reader.read(value);
            printf("%f", value);
        } break;

        case kError: {
            uint8_t code;
            reader.readError(code);
            printf("%u 0x%02X ", code, code);

            int16_t data;
            reader.read(data);
            printf("%d", data);
        } break;

        case kEndOfMessage: {
            printf("CRC %s", reader.readCrc() ? "Valid" : "Invalid");
        } break;

        default: {
            printf("Unknown Type");
        } break;
        }
        printf("\n");
        ++param;
    }
    printf("\n");
}

void process(std::istream& stream)
{
    DecodeBuffer buffer;

    uint16_t byte = 0;
    while (stream >> std::hex >> byte)
    {
        if (byte > 0xFF)
        {
            throw std::runtime_error("Read a value greater than the size of a byte");
        }
        
        buffer.writeByte(byte);
        
        if (buffer.messages())
        {
            decode(buffer);
        }
    }
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        process(std::cin);
    }
    else if (argc == 2)
    {
        std::ifstream file(argv[1]);
        process(file);
    }
    else
    {
        return 1;
    }


    return 0;
}