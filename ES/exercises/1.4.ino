#include <Arduino.h>

#define LED 13
#define BUTTON 3

bool ledState = LOW;

void flipLED() { digitalWrite(LED, ledState = !ledState); }

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON), flipLED, RISING);
}

void loop()
{
}