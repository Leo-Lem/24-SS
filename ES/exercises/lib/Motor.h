#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#include "RGBLED.h"

class Motor
{
public:
  static const unsigned int step = 15, max = 255;

  enum Direction
  {
    stopped,
    clockwise,
    counterclockwise
  };

  enum Mode
  {
    POWER,
    DIRECTION
  };

  Mode mode = DIRECTION;
  Direction direction = stopped;
  unsigned long power = 0;

  // mock constructor
  Motor() : pwm(9), in1(14), in2(15), led(RGBLED(11, 7, 6)) {}

  Motor(unsigned int pwm, unsigned int in1, unsigned int in2, RGBLED led) : pwm(pwm), in1(in1), in2(in2), led(led)
  {
    pinMode(pwm, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(pwm, power);
  }

  void toggle()
  {
    mode = mode == POWER ? DIRECTION : POWER;
    status(true, false);
    updateLED();
  }

  void adjust(bool increase = false)
  {
    bool powerLimitReached = false;

    if (mode == DIRECTION)
      setDirection(increase);
    else
      powerLimitReached = setPower(increase);

    status(false, powerLimitReached);
    updateLED();
  }

private:
  const unsigned int pwm, in1, in2;
  RGBLED led;

  void setDirection(bool forward = false)
  {
    switch (direction)
    {
    case stopped:
      direction = forward ? clockwise : counterclockwise;
      break;
    default:
      direction = stopped;
    }

    switch (direction)
    {
    case stopped:
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      break;
    case clockwise:
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      break;
    case counterclockwise:
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    }
  }

  bool setPower(bool increase = false) // returns true if power has reached lower/upper limit
  {
    bool limitReached = false;

    if (increase && power < max)
      power += step;
    else if (!increase && power > 0)
      power -= step;
    else
      limitReached = true;

    analogWrite(pwm, power);

    return limitReached;
  }

  void updateLED()
  {
    if (mode == DIRECTION && direction == stopped)
      led.setColor(RGBLED::blue);
    else
      led.setColor(mode == POWER ? RGBLED::red : RGBLED::green);
  }

  void status(bool newMode, bool powerLimitReached)
  {
    Serial.print("[");
    if (newMode)
      Serial.print(">");
    Serial.print(mode == POWER ? "POWER" : "DIRECTION");
    Serial.print(" MODE] Motor direction: ");
    Serial.print(direction == stopped ? "CW - (STOP) - CCW" : direction == clockwise ? "(CW) - STOP - CCW"
                                                                                     : "CW - STOP - (CCW)");
    Serial.print(" | Motor power: ");
    Serial.print(min(power * 100 / max, 100));
    Serial.print("%");

    if (powerLimitReached)
      Serial.print(" (limit reached)");

    Serial.println("");
  }
};

#endif