#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <EmbMessenger/Command.hpp>

class Ping : public emb::Command
{
};

class SetLed : public emb::Command
{
    bool m_ledState;

public:
    SetLed(bool ledState);
    void send(emb::EmbMessenger* messenger);
};

class ToggleLed : public emb::Command
{
    bool m_ledState;

public:
    ToggleLed();
    void receive(emb::EmbMessenger* messenger);
    bool getLedState() const;
};

class Add : public emb::Command
{
    int16_t m_a, m_b, m_rv;

public:
    Add(int16_t a, int16_t b);
    void send(emb::EmbMessenger* messenger);
    void receive(emb::EmbMessenger* messenger);
    int16_t getA() const;
    int16_t getB() const;
    int16_t getResult() const;
};

class DelayMs : public emb::Command
{
    uint16_t m_ms;

public:
    DelayMs(uint16_t ms);
    void send(emb::EmbMessenger* messenger);
    uint16_t getDelayTime() const;
};

#endif // COMMANDS_HPP
