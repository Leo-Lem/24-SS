#include <Arduino.h>
#define LED 13
#define BUTTON 3

enum State
{
  idle,
  active,
  pressed,
  released
};

static volatile bool ledState = LOW;
static State state = idle;

static void flip()
{
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

static State newState()
{
}

void setup33()
{
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON), flip, CHANGE);
}

void loop33()
{
}