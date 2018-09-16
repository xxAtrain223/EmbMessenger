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
            void reportError(const uint8_t error, std::shared_ptr<Command> ptr)
            {
                switch (error)
                {
                case 0x42:
                    throw UserErrorException("Something happened on the device", ptr);
                }
            }

        public:
            UserError()
            {
                m_type_index = typeid(UserError);
            }
        };
    }
}

#endif // EMBMESSENGER_TEST_USERERROR_HPP