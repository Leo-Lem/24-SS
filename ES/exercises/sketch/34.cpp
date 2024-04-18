#include <Arduino.h>
#define BUTTON1 3
#define BUTTON2 4
#define LED 13

const static int frequency = 1000;                  // Hz
const static long period = (long)(1e6 / frequency); // µs
const static long debounceTime = 10;                // ms

static long lastDebounceTime[2] = {0, 0};
static bool buttonState[2] = {HIGH, HIGH};
static bool lastButtonState[2] = {HIGH, HIGH};
static bool blink = false;
static long doublePresses = 0;

static bool debounce(bool isButton1)
{
  bool reading = digitalRead(isButton1 ? BUTTON1 : BUTTON2);

  if (reading != lastButtonState[isButton1])
    lastDebounceTime[isButton1] = millis();

  lastButtonState[isButton1] = reading;

  if ((millis() - lastDebounceTime[isButton1]) > debounceTime)
  {
    if (reading != buttonState[isButton1])
    {
      buttonState[isButton1] = reading;
      return true;
    }
  }

  return false;
}

static void tick()
{
  bool isDebounced1 = debounce(true);
  bool isDebounced2 = debounce(false);

  if (isDebounced1 && isDebounced2 && buttonState[0] == HIGH && buttonState[1] == HIGH)
  {
    blink = true;
    Serial.println("Both buttons pressed");
    doublePresses++;
    Serial.print("Double presses: ");
    Serial.println(doublePresses);
  }
  else if (isDebounced1 && buttonState[1] == HIGH)
  {
    blink = false;
    digitalWrite(LED, HIGH);
    Serial.println("Button 1 pressed");
  }
  else if (isDebounced2 && buttonState[0] == HIGH)
  {
    blink = false;
    digitalWrite(LED, LOW);
    Serial.println("Button 2 pressed");
  }
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

void setup34()
{
  configureTimer();
  Serial.begin(9600);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  Timer4.start();
}

void loop34()
{
  if (blink)
  {
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }
}