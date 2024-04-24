#include <Arduino.h>

#if defined(__SAM3X8E__) // Arduino Due
#include "DueTimer.h"
static DueTimer Timer4; // match TimerFour package
#else
#include <TimerFour.h>
#endif

#define BUTTON 3
#define LED 13

const static int frequency = 1000;                  // Hz
const static long period = (long)(1e6 / frequency); // µs
const static long debounceInterval = 20;            // ms

enum ButtonState
{
  released,
  active,
  pressed,
  idle
};

static ButtonState state = released;
static int debounceTime = 0;
static int presses = 0;

static void updateState(bool reading, bool debounced)
{
  switch (state)
  {
  case released: // button is released
    if (reading == LOW)
      state = active;
    break;
  case active: // high signal detected -> debounce
    if (reading == HIGH)
      state = released;
    else if (debounced)
      state = pressed;
    break;
  case pressed: // button is pressed
    if (reading == HIGH)
      state = idle;
    break;
  case idle: // low signal detected -> debounce
    if (reading == LOW)
      state = pressed;
    else if (debounced)
      state = released;
    break;
  }
}

static void press()
{
  presses++;
  Serial.print("Button pressed ");
  Serial.print(presses);
  Serial.println(" times.");
}

static void debounce()
{
  updateState(digitalRead(BUTTON), debounceTime > debounceInterval);

  if (state == active || state == idle)
    debounceTime += period / 1000;
  else
  {
    if (state == released && debounceTime > debounceInterval)
      press();
    debounceTime = 0;
    Timer4.stop(); // stop debounce
  }
}

static void tick()
{
  debounce();
}

static void configureTimer()
{
#if defined(__SAM3X8E__) // Arduino Due
  if (!Timer4.configure(frequency, tick))
    Serial.println("Failed to start timer…");
#else
  Timer4.initialize(period);
  Timer4.attachInterrupt(tick);
  Timer4.stop(); // attachInterrupt starts Timer > stop it
#endif
}

static void buttonISR()
{
  Timer4.start(); // start debounce
}

void setup33()
{
  configureTimer();
  Serial.begin(9600);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, CHANGE);

  Serial.println("Press the button to get started!");
}

void loop33() {}