#include "Commands.hpp"
#include <EmbMessenger/EmbMessenger.hpp>

SetLed::SetLed(bool ledState)
{
    m_ledState = ledState;
    m_type_index = typeid(SetLed);
}

void SetLed::send(emb::host::EmbMessenger* messenger)
{
    messenger->write(m_ledState);
}

ToggleLed::ToggleLed()
{
    m_type_index = typeid(ToggleLed);
}

void ToggleLed::receive(emb::host::EmbMessenger* messenger)
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

void Add::send(emb::host::EmbMessenger* messenger)
{
    messenger->write(m_a, m_b);
}

void Add::receive(emb::host::EmbMessenger* messenger)
{
    messenger->read(m_rv);
}

int16_t Add::getA() const
{
    return m_a;
}

int16_t Add::getB() const
{
    return m_b;
}

int16_t Add::getResult() const
{
    return m_rv;
}

DelayMs::DelayMs(uint16_t ms)
{
    m_ms = ms;
}

void DelayMs::send(emb::host::EmbMessenger* messenger)
{
    messenger->write(m_ms);
}

uint16_t DelayMs::getDelayTime() const
{
    return m_ms;
}