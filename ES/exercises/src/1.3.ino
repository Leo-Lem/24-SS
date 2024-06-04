#include <Arduino.h>

#define LED 13
#define BUTTON 3

volatile bool ledState = LOW;
bool buttonClicked = false;

void flip()
{
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

void check()
{
  const bool buttonState = digitalRead(BUTTON);
  if (!buttonState && !buttonClicked)
  {
    flip();
    buttonClicked = true;
  }
  else if (buttonState && buttonClicked)
    buttonClicked = false;
}

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop()
{
  check();
  delay(10);
}