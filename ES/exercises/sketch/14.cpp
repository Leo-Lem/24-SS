#include <Arduino.h>
#define LED 13
#define BUTTON 3

static bool ledState = LOW;

static void flipLED()
{
  digitalWrite(LED, ledState = !ledState);
}

void setup14()
{
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON), flipLED, RISING);
}

void loop14()
{
}