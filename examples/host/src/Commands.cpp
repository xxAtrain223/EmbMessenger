#include "Commands.hpp"
#include <EmbMessenger/EmbMessenger.hpp>

SetLed::SetLed(bool ledState)
{
    m_ledState = ledState;
    m_type_index = typeid(SetLed);
}

void SetLed::send(emb::EmbMessenger* messenger)
{
    messenger->write(m_ledState);
}

ToggleLed::ToggleLed()
{
    m_type_index = typeid(ToggleLed);
}

void ToggleLed::receive(emb::EmbMessenger* messenger)
{
    messenger->read(m_ledState);
}

bool ToggleLed::getLedState() const
{
    return m_ledState;
}

Add::Add(int16_t a, int16_t b)
{
    m_a = a;
    m_b = b;
    m_rv = 0;
}

void Add::send(emb::EmbMessenger* messenger)
{
    messenger->write(m_a, m_b);
}

void Add::receive(emb::EmbMessenger* messenger)
{
    messenger->read(m_rv);
}

int16_t Add::getResult() const
{
    return m_rv;
}
