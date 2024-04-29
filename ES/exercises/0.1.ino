#include <Arduino.h>

#define LED 13

bool ledState = LOW;

void flip()
{
  ledState = !ledState;
}

void setup() { pinMode(LED, OUTPUT); }

void loop()
{
  flip();
  digitalWrite(LED, ledState);
  delay(400);
  flip();
  digitalWrite(LED, ledState);
  delay(200);
}