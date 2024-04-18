#include <Arduino.h>
#define LED 13

static bool ledState = LOW;

static void flip()
{
  ledState = !ledState;
}

void setup01()
{
  pinMode(LED, OUTPUT);
}

void loop01()
{
  flip();
  digitalWrite(LED, ledState);
  delay(400);
  flip();
  digitalWrite(LED, ledState);
  delay(200);
}