#include <Arduino.h>

class Motor
{
public:
  static const unsigned int step = 10, max = 255;

  enum Rotation
  {
    stopped,
    clockwise,
    counterclockwise
  };

  enum Setting
  {
    pow, // power
    rot  // rotate
  };

  Setting setting = rot;
  Rotation rotation = stopped;
  unsigned long power = 0;

  void toggle() { setting = setting == pow ? rot : pow; }

  void adjust(bool increase = false)
  {
    if (setting == rot)
      setRotation(increase);
    else
      setPower(increase);
  }

private:
  void setRotation(bool forward = false)
  {
    if (forward) // stopped -> counterclockwise -> clockwise -> stopped
      rotation == stopped ? rotation = counterclockwise : rotation == counterclockwise ? rotation = clockwise
                                                                                       : rotation = stopped;
    else // stopped -> clockwise -> counterclockwise -> stopped
      rotation == stopped ? rotation = clockwise : rotation == clockwise ? rotation = counterclockwise
                                                                         : rotation = stopped;
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
};