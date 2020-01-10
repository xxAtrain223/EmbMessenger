#include <EmbMessenger/EmbMessenger.hpp>
#include "ArduinoBuffer.hpp"

ArduinoBuffer<64> buffer(&Serial);
extern emb::device::EmbMessenger<1> messenger;
bool ledState = false;
const uint8_t ledPin = LED_BUILTIN;

void setup()
{
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
}

void loop()
{
    messenger.update();
}

void ping()
{
}

void setLedAdaptor()
{
    bool state = false;
    messenger.read(state);
    setLed(state);
}

void setLed(bool value)
{
    ledState = value;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
}

void toggleLedAdaptor()
{
    messenger.write(toggleLed());
}

bool toggleLed()
{
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
    return ledState;
}

void addAdaptor()
{
    int16_t a = 0, b = 0, rv = 0;

    messenger.read(a, [](int16_t val){ return val > -16 && val < 128; });
    messenger.read(b, [](int16_t val){ return val > -16 && val < 128; });

    add(a, b, rv);

    messenger.write(rv);
}

void add(int a, int b, int& rv)
{
    rv = a + b;
}

void delayMs()
{
    uint16_t ms = 0;
    messenger.read(ms);

    delay(ms);
}

const emb::device::EmbMessenger<>::CommandFunction commands[] PROGMEM = {
    ping,
    setLedAdaptor,
    toggleLedAdaptor,
    addAdaptor,
    delayMs
};
emb::device::EmbMessenger<1> messenger(&buffer, commands, 5, millis);
