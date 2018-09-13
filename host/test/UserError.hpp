#ifndef EMBMESSENGER_TEST_USERERROR_HPP
#define EMBMESSENGER_TEST_USERERROR_HPP

#include "EmbMessenger/Command.hpp"
#include "EmbMessenger/Exceptions.hpp"

namespace emb
{
    namespace test
    {
        NEW_DEVICE_EX(0x42, UserErrorException);

        class UserError : public Command
        {
            void reportError(const uint8_t error)
            {
                switch (error)
                {
                case 0x42:
                    throw UserErrorException("Something happened on the device");
                }
            }
        };
    }
}

#endif // EMBMESSENGER_TEST_USERERROR_HPP