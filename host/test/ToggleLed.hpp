#ifndef EMBMESSENGER_TEST_TOGGLELED_HPP
#define EMBMESSENGER_TEST_TOGGLELED_HPP

#include "EmbMessenger/Command.hpp"

namespace emb
{
    namespace test
    {
        class ToggleLed : public Command
        {
        public:
            bool ledState;

            void receive(EmbMessenger* messenger)
            {
                messenger->read(ledState);
            }
        };
    }
}

#endif // EMBMESSENGER_TEST_TOGGLELED_HPP