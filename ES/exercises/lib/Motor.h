#include <Arduino.h>

#include "RGBLED.h"

class Motor
{
public:
  static const unsigned int step = 10, max = 255;

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

  Motor(RGBLED led) : led(led) {}

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
      setRotation(increase);
    else
      powerLimitReached = !setPower(increase);

    status(false, powerLimitReached);
    updateLED();
  }

private:
  RGBLED led;

  void setRotation(bool forward = false)
  {
    if (forward) // stopped -> counterclockwise -> clockwise -> stopped
      direction == stopped ? direction = counterclockwise : direction == counterclockwise ? direction = clockwise
                                                                                          : direction = stopped;
    else // stopped -> clockwise -> counterclockwise -> stopped
      direction == stopped ? direction = clockwise : direction == clockwise ? direction = counterclockwise
                                                                            : direction = stopped;
  }

  bool setPower(bool increase = false) // returns false if power has reached lower/upper limit
  {
    if (increase && power < max)
    {
      power += step;
      return true;
    }
    else if (!increase && power > 0)
    {
      power -= step;
      return true;
    }
    else
      return false;
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
    Serial.print("\033[K");

    Serial.print("[");
    if (newMode)
      Serial.print("> ");
    Serial.print(mode == POWER ? "POWER" : "DIRECTION");
    Serial.print(" MODE] Motor direction: ");
    Serial.print(direction == stopped ? "CW - (STOP) - CCW" : direction == clockwise ? "(CW) - STOP - CCW"
                                                                                     : "CW - STOP - (CCW)");
    Serial.print(" | Motor power: ");
    Serial.print(min(power * 100 / max, 100));
    Serial.print("%");

    if (powerLimitReached)
      Serial.print(" (limit reached)");

    Serial.print("\r");
  }
};