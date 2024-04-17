#if defined(__SAM3X8E__) // Arduino Due
#include <DueTimer.h>
DueTimer Timer4; // match TimerFour timer descriptor
#else
#include <TimerFour.h>
#endif

#define buttonPin 2

const unsigned long frequency = 1e3;          // Hz
const unsigned long period = 1e6 / frequency; // µs
volatile unsigned long buttonPressDuration = 0;
volatile bool buttonPressed = false;

void buttonPressedISR()
{
  buttonPressed = digitalRead(buttonPin) == HIGH;

  if (!buttonPressed)
  {
    Serial.print("Button pressed for ");
    Serial.println(buttonPressDuration);
    buttonPressDuration = 0;
  }
}

void timerISR()
{
  if (buttonPressed)
  {
    buttonPressDuration++;
    Serial.println(buttonPressDuration);
  }
}

void startTimer()
{
#if defined(__SAM3X8E__) // Arduino Due
  if (Timer4.configure(frequency, timerISR))
  {
    ;
  }
  else
  {
    Serial.println("Failed to start timer…");
  }
#else
  Timer4.initialize(period);
  Timer4.attachInterrupt(timerISR);
  Timer4.stop(); // attachInterrupt starts Timer > stop it
#endif

  Timer4.start();
}

void setup31()
{
  startTimer();
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPressedISR, CHANGE);

  Serial.println("Up and running!");
}

void loop31()
{
}
