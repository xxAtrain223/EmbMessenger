#ifndef EMBMESSENGER_TEST_USERERROR_HPP
#define EMBMESSENGER_TEST_USERERROR_HPP

#include "EmbMessenger/Command.hpp"
#include "EmbMessenger/Exceptions.hpp"

namespace emb
{
    namespace host
    {
        namespace test
        {
            NEW_EMB_EX_SOURCE(UserErrorException, Device);

            class UserError : public Command
            {
                void reportError(const uint8_t error, const int16_t data, std::shared_ptr<Command> ptr)
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
        }  // namespace test
    }  // namespace host
}  // namespace emb

#endif  // EMBMESSENGER_TEST_USERERROR_HPP