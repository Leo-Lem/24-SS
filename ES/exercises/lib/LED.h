#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LED
{
public:
  enum class State
  {
    off,
    on,
    blinking
  };

  State state = State::off;

  LED(int pin) : pin(pin) { pinMode(pin, OUTPUT); }

  void update(State state)
  {
    this->state = state;
    if (state != State::blinking)
      digitalWrite(pin, state == State::on ? HIGH : LOW);
  }

  void blink()
  {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
  }

private:
  const int pin;
};

#endif