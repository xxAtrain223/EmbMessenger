#ifndef EMBMESSENGER_TEST_ADD_HPP
#define EMBMESSENGER_TEST_ADD_HPP

#include "EmbMessenger/Command.hpp"
#include "EmbMessenger/EmbMessenger.hpp"

namespace emb
{
    namespace test
    {
        class Add : public Command
        {
        protected:
            int A, B;

        public:
            int Result;

            Add(int a, int b)
            {
                A = a;
                B = b;
            }

            void send(EmbMessenger* messenger)
            {
                messenger->write(A, B);
            }

            void receive(EmbMessenger* messenger)
            {
                messenger->read(Result);
            }
        };
    }
}

#endif // EMBMESSENGER_TEST_SETLED_HPP