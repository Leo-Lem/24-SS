#include <Arduino.h>

#include "Button.h"

class DoubleButton
{
public:
  DoubleButton(int pin1, int pin2, void (*press1)(), void (*press2)(), void (*pressBoth)()) : button{Button(pin1, press1), Button(pin2, press2)}, pressBoth(pressBoth) {}

  void update()
  {
    if (button[0].isWaiting() && button[1].isWaiting())
      pressBoth();

    for (auto &b : button)
      (button[0].isWaiting() && button[1].isWaiting()) ? b.cancel() : b.update();
  }

private:
  Button button[2];
  void (*pressBoth)();
};