#include "EmbeddedMessenger/Reader.hpp"

namespace emb
{
    bool Reader::peekByte(uint8_t & value) const
    {
        return false;
    }

    bool Reader::readByte(uint8_t & value)
    {
        return false;
    }

    bool Reader::removeByte()
    {
        return false;
    }

    Reader::Reader(IBuffer * buffer)
    {
    }

    DataType Reader::getType() const
    {
        return DataType();
    }

    bool Reader::nextBool() const
    {
        return false;
    }

    bool Reader::nextChar() const
    {
        return false;
    }

    bool Reader::nextSignedInt() const
    {
        return false;
    }

    bool Reader::nextUnsignedInt() const
    {
        return false;
    }

    bool Reader::nextInt() const
    {
        return false;
    }

    bool Reader::nextFloat() const
    {
        return false;
    }

    bool Reader::nextNull() const
    {
        return false;
    }

    bool Reader::nextError() const
    {
        return false;
    }

    bool Reader::nextCrc() const
    {
        return false;
    }

    bool Reader::read(bool & value)
    {
        return false;
    }

    bool Reader::read(uint8_t & value)
    {
        return false;
    }

    bool Reader::read(uint16_t & value)
    {
        return false;
    }

    bool Reader::read(uint32_t & value)
    {
        return false;
    }

    bool Reader::read(uint64_t & value)
    {
        return false;
    }

    bool Reader::read(int8_t & value)
    {
        return false;
    }

    bool Reader::read(int16_t & value)
    {
        return false;
    }

    bool Reader::read(int32_t & value)
    {
        return false;
    }

    bool Reader::read(int64_t & value)
    {
        return false;
    }

    bool Reader::read(float & value)
    {
        return false;
    }

    bool Reader::readNull()
    {
        return false;
    }

    bool Reader::readError(uint8_t & value)
    {
        return false;
    }

    bool Reader::readCrc()
    {
        return false;
    }

    void Reader::resetCrc()
    {
    }
}