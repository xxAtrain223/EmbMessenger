#include <EmbMessenger/EmbMessenger.hpp>
#include "ArduinoBuffer.hpp"

ArduinoBuffer<64> buffer(&Serial);
emb::EmbMessenger<5, 1> messenger(&buffer, millis);
bool ledState = false;
const uint8_t ledPin = LED_BUILTIN;

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

    messenger.read_and_validate(a, [](int16_t val){ return val > -16 && val < 128; });
    messenger.read_and_validate(b, [](int16_t val){ return val > -16 && val < 128; });

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

void setup()
{
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);

    messenger.attachCommand(0, ping);
    messenger.attachCommand(1, setLedAdaptor);
    messenger.attachCommand(2, toggleLedAdaptor);
    messenger.attachCommand(3, addAdaptor);
    messenger.attachCommand(4, delayMs);
}

void loop()
{
    messenger.update();
}
