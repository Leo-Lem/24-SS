#ifndef RGBLED_H
#define RGBLED_H

#include <Arduino.h>

class RGBLED
{
public:
  enum Color
  {
    red = 0,
    green = 1,
    blue = 2
  };

  Color color = green;

  RGBLED(unsigned int redPin, unsigned int greenPin, unsigned int bluePin)
      : pins{redPin, greenPin, bluePin}
  {
    for (unsigned int pin : pins)
      pinMode(pin, OUTPUT);
  }

  void setColor(Color color)
  {
    this->color = color;

    if (color == blue)
      analogWrite(pins[blue], LOW);
    else
      for (Color c : colors)
        digitalWrite(pins[c], c == color ? LOW : HIGH);
  }

private:
  const Color colors[3] = {red, green, blue};
  const unsigned int pins[3]; // red, green, blue
};

#endif