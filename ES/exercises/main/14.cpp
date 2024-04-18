#include <Arduino.h>
#define LED 13
#define BUTTON 3

static volatile bool ledState = LOW;

static void flip()
{
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

void setup14()
{
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON), flip, CHANGE);
}

void loop14()
{
}