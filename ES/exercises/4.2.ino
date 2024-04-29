#include <Arduino.h>

#include "DoubleButton.h"
#include "Motor.h"

#define BUTTON0PIN 2
#define BUTTON1PIN 3
#define LEDPINR 11
#define LEDPING 7
#define LEDPINB 6

RGBLED led = RGBLED(LEDPINR, LEDPING, LEDPINB);
Motor motor = Motor(led);

DoubleButton button = DoubleButton(
    BUTTON0PIN, BUTTON1PIN,
    [](void)
    {
      motor.adjust(true);
    },
    [](void)
    {
      motor.adjust(false);
    },
    [](void)
    {
      motor.toggle();
    });

void tick() { button.update(); }

void setup()
{
  Serial.begin(9600);

  Timer::configure(tick); // configure the timer to update our button state
}

void loop() {}