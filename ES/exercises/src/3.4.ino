#include <Arduino.h>

#include "DoubleButton.h"
#include "LED.h"

#define BUTTON0PIN 2
#define BUTTON1PIN 3
#define LEDPIN 13

LED led = LED(LEDPIN);

DoubleButton button = DoubleButton(
    BUTTON0PIN, BUTTON1PIN,
    [](void)
    {
      led.update(LED::State::on);
      Serial.println("Button 1 pressed.");
    },
    [](void)
    {
      led.update(LED::State::off);
      Serial.println("Button 2 pressed.");
    },
    [](void)
    {
      led.update(LED::State::blinking);
      Serial.println("Both buttons pressed.");
    });

void tick() { button.update(); }

void setup()
{
  Serial.begin(9600);

  Timer::configure(tick); // configure the timer to update our button state

  Serial.println("Press buttons to change LED state:");
  Serial.println("  Button 1: LED on");
  Serial.println("  Button 2: LED off");
  Serial.println("  Both buttons: LED blinking");
}

void loop()
{
  if (led.state == LED::State::blinking)
    led.blink();
}