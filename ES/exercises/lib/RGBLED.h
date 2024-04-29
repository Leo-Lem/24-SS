#include <Arduino.h>

class RGBLED
{
public:
  enum Color
  {
    red,
    green,
    blue
  };

  Color color = green;

  RGBLED(unsigned int redPin, unsigned int greenPin, unsigned int bluePin)
      : pins{redPin, greenPin, bluePin}
  {
    for (auto &pin : pins)
      pinMode(pin, OUTPUT);
  }

  void setColor(Color color)
  {
    this->color = color;

    for (unsigned int i = 0; i < 3; i++)
      digitalWrite(pins[i], i == color ? HIGH : LOW);
  }

private:
  const unsigned int pins[3]; // red, green, blue
};