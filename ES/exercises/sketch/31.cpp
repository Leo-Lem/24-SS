#include <Arduino.h>
#define BUTTON 3

const static int frequency = 1000;                  // Hz
const static long period = (long)(1e6 / frequency); // µs

static long duration = 0; // ms
static bool buttonPressed = false;

static void tick()
{
  if (buttonPressed)
    duration += period / 1000; // ms
}

#if defined(__SAM3X8E__) // Arduino Due
#include "DueTimer.h"
static DueTimer Timer4; // match TimerFour package
static void configureTimer()
{
  if (!Timer4.configure(frequency, tick))
    Serial.println("Failed to start timer…");
}
#else
#include <TimerFour.h>
static void configureTimer()
{
  Timer4.initialize(period);
  Timer4.attachInterrupt(tick);
  Timer4.stop(); // attachInterrupt starts Timer > stop it
}
#endif

static void press()
{
  buttonPressed = digitalRead(BUTTON) == LOW;
}

void setup31()
{
  configureTimer();
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), press, CHANGE);

  Timer4.start();

  Serial.println("Press the button to get started!");
}

void loop31()
{
  if (!buttonPressed && duration > 0)
  {
    Serial.print("Button pressed for ");
    Serial.print(duration);
    Serial.println("ms.");

    duration = 0;
  }
}
