#ifndef EMBMESSENGER_TEST_TOGGLELED_HPP
#define EMBMESSENGER_TEST_TOGGLELED_HPP

#include "EmbMessenger/Command.hpp"

namespace emb
{
    namespace host
    {
        namespace test
        {
            class ToggleLed : public Command
            {
            public:
                bool ledState;

                ToggleLed()
                {
                    m_type_index = typeid(ToggleLed);
                }

                void receive(EmbMessenger* messenger)
                {
                    messenger->read(ledState);
                }
            };
        }  // namespace test
    }  // namespace host
}  // namespace emb

#endif  // EMBMESSENGER_TEST_TOGGLELED_HPP