#ifndef EMBMESSENGER_TEST_PING_HPP
#define EMBMESSENGER_TEST_PING_HPP

#include "EmbMessenger/Command.hpp"

namespace emb
{
    namespace host
    {
        namespace test
        {
            class Ping : public Command
            {
            public:
                Ping()
                {
                    m_type_index = typeid(Ping);
                }
            };
        }  // namespace test
    }  // namespace host
}  // namespace emb

#endif  // EMBMESSENGER_TEST_PING_HPP