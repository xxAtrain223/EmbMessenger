#ifndef EMBMESSENGER_TEST_SETLED_HPP
#define EMBMESSENGER_TEST_SETLED_HPP

#include "EmbMessenger/Command.hpp"

namespace emb
{
    namespace test
    {
        class SetLed : public Command
        {
           protected:
            bool ledState;

           public:
            SetLed(bool state)
            {
                ledState = state;
                m_type_index = typeid(SetLed);
            }

            void send(EmbMessenger* messenger)
            {
                messenger->write(ledState);
            }
        };
    }  // namespace test
}  // namespace emb

#endif  // EMBMESSENGER_TEST_SETLED_HPP