#include <Arduino.h>
#define LED 13
#define BUTTON 3

static volatile bool ledState = LOW;
static bool buttonClicked = false;

static void flip()
{
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

static void check()
{
  const bool buttonState = digitalRead(BUTTON);
  if (!buttonState && !buttonClicked)
  {
    flip();
    buttonClicked = true;
  }
  else if (buttonState && buttonClicked)
  {
    buttonClicked = false;
  }
}

void setup13()
{
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop13()
{
  check();
  delay(10);
}