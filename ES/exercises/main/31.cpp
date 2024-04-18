#include <Arduino.h>

#if defined(__SAM3X8E__) // Arduino Due
#include <DueTimer.h>
DueTimer Timer4; // match TimerFour timer descriptor
#else
#include <TimerFour.h>
#endif

#define BUTTON 3

const unsigned long frequency = 1e3;          // Hz
const unsigned long period = 1e6 / frequency; // µs
volatile unsigned long buttonPressDuration = 0;
volatile bool buttonPressed = false;

static void press()
{
  buttonPressed = digitalRead(BUTTON) == LOW;

  if (!buttonPressed)
  {
    Serial.print("Button pressed for ");
    Serial.print(buttonPressDuration);
    Serial.println(" ms.");

    buttonPressDuration = 0;
  }
}

static void time()
{
  if (buttonPressed)
  {
    buttonPressDuration++;
  }
}

static void startTimer()
{
#if defined(__SAM3X8E__) // Arduino Due
  if (Timer4.configure(frequency, time))
  {
    ;
  }
  else
  {
    Serial.println("Failed to start timer…");
  }
#else
  Timer4.initialize(period);
  Timer4.attachInterrupt(time);
  Timer4.stop(); // attachInterrupt starts Timer > stop it
#endif

  Timer4.start();
}

void setup31()
{
  startTimer();
  Serial.begin(9600);

  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), press, CHANGE);

  Serial.println("Press the button to get started!");
}

void loop31()
{
}
