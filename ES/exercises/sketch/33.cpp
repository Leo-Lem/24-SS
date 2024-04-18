#include <Arduino.h>
#define BUTTON 3
#define LED 13

const static int frequency = 1000;                  // Hz
const static long period = (long)(1e6 / frequency); // µs
const static long debounce = 10;                    // ms

static long lastDebounceTime = 0;
static bool buttonState = HIGH;
static bool lastButtonState = HIGH;

static int presses = 0;

static void tick()
{
  bool reading = digitalRead(BUTTON);

  if (reading != lastButtonState)
    lastDebounceTime = millis();

  if ((millis() - lastDebounceTime) > debounce)
  {
    if (reading != buttonState)
    {
      buttonState = reading;

      if (buttonState == HIGH)
      {
        digitalWrite(LED, !digitalRead(LED));
        Serial.print("Presses: ");
        Serial.println(++presses);
      }
    }
  }

  lastButtonState = reading;
}

#if defined(__SAM3X8E__) // Arduino Due
#include <DueTimer.h>
DueTimer Timer4; // match TimerFour package
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

void setup33()
{
  configureTimer();
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  Timer4.start();
}

void loop33()
{
}