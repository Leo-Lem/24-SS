#include <Arduino.h>
#define POT 9
#define LED 13

static void modulate()
{
  int value = analogRead(POT); // 0-1023
  int duty = value / 4;        // scale to duty (0-255) 1/4~255/1023

  analogWrite(LED, duty);
}

void setup23()
{
  pinMode(LED, OUTPUT);
}

void loop23()
{
  modulate();
  delay(100);
}