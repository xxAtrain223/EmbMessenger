#ifndef EMBEDDEDMESSENGER_TEST_MOCKBUFFER_HPP
#define EMBEDDEDMESSENGER_TEST_MOCKBUFFER_HPP

#include <gmock/gmock.h>

#include "EmbeddedMessenger/IBuffer.hpp"

namespace emb
{
    namespace test
    {
        class MockBuffer : public IBuffer
        {
        public:
            MOCK_METHOD1(writeByte, void(const uint8_t& byte));
            MOCK_CONST_METHOD0(peek, uint8_t());
            MOCK_METHOD0(readByte, uint8_t());
            MOCK_CONST_METHOD0(empty, bool());
            MOCK_CONST_METHOD0(size, size_t());
            MOCK_CONST_METHOD0(messageAvailable, bool());
            MOCK_METHOD0(update, void());
        };
    }
}

#endif // EMBEDDEDMESSENGER_TEST_MOCKBUFFER_HPP