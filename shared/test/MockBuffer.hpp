#ifndef EMBMESSENGER_TEST_MOCKBUFFER_HPP
#define EMBMESSENGER_TEST_MOCKBUFFER_HPP

#include <gmock/gmock.h>

#include "EmbMessenger/IBuffer.hpp"

namespace emb
{
    namespace test
    {
        class MockBuffer : public IBuffer
        {
        public:
            MOCK_METHOD1(writeByte, void(const uint8_t byte));
            MOCK_CONST_METHOD0(peek, uint8_t());
            MOCK_METHOD0(readByte, uint8_t());
            MOCK_CONST_METHOD0(empty, bool());
            MOCK_CONST_METHOD0(size, size_t());
            MOCK_CONST_METHOD0(messagesAvailable, uint8_t());
            MOCK_METHOD0(update, void());
            MOCK_METHOD0(zero, void());
        };
    }
}

#endif // EMBMESSENGER_TEST_MOCKBUFFER_HPP