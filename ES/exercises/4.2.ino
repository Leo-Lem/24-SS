#include <Arduino.h>

#include "DoubleButton.h"
#include "Motor.h"
#include "RGBLED.h"

#define BUTTON0PIN 2
#define BUTTON1PIN 3
#define LEDRPIN 11
#define LEDGPIN 7
#define LEDBPIN 6

Motor motor = Motor();
RGBLED led = RGBLED(LEDRPIN, LEDGPIN, LEDBPIN);

DoubleButton button = DoubleButton(
    BUTTON0PIN, BUTTON1PIN,
    [](void)
    {
      motor.adjust(true);
      Serial.println("Button 1 pressed.");
    },
    [](void)
    {
      motor.adjust(false);
      Serial.println("Button 2 pressed.");
    },
    [](void)
    {
      motor.toggle();
      Serial.println("Both buttons pressed.");
    });

void tick() { button.update(); }

void setup()
{
  Serial.begin(9600);

  Timer::configure(tick); // configure the timer to update our button state
}

void loop() {}