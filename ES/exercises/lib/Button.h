

#include <Arduino.h>
#include "Timer.h"

#ifndef BUTTON_H
#define BUTTON_H

class Button
{
public:
  static const unsigned int debounceInterval = 20, waitInterval = 100;

  enum State
  {
    released,
    active,
    pressed,
    idle
  };

  State state = released;
  void (*press)();

  Button(int pin, void (*press)()) : pin(pin), press(press)
  {
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin), Timer::start, CHANGE);
  }

  bool isDebouncing() { return debounceTime > -1; }
  bool isWaiting() { return waitTime > -1; }

  void update()
  {
    switch (state)
    {
    case released: // button is released
      if (read() == LOW)
      {
        state = active;
        debounceTime = 0; // start debouncing
      }
      break;
    case active: // low signal detected
      if (read() == HIGH)
        state = released;
      break;
    case pressed: // button is pressed
      if (read() == HIGH)
      {
        state = idle;
        debounceTime = 0; // start debouncing
      }
      break;
    case idle: // high signal detected
      if (read() == LOW)
        state = pressed;
    }

    if (isDebouncing())
      debounce();
    else if (isWaiting())
      wait();
  };

  void cancel()
  {
    noInterrupts();
    waitTime = -1;
    Timer::stop();
    interrupts();
  }

private:
  const unsigned int pin;
  long debounceTime = -1; // ms (-1: not debouncing)
  long waitTime = -1;     // ms (-1: not waiting)

  bool read() { return digitalRead(pin); }
  void debounce()
  {
    debounceTime += Timer::periodToMs;

    if (debounceTime > debounceInterval)
    {
      if (state == active)
        state = pressed;
      else if (state == idle)
      {
        state = released;
        waitTime = 0; // start waiting
      }

      debounceTime = -1;
    }
  }
  void wait()
  {
    waitTime += Timer::periodToMs;

    if (waitTime > waitInterval)
    {
      press();
      cancel();
    }
  }
};

#endif