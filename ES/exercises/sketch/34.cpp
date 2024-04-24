#include <Arduino.h>

#if defined(__SAM3X8E__) // Arduino Due
#include "DueTimer.h"
static DueTimer Timer4; // match TimerFour package
#else
#include <TimerFour.h>
#endif

#define BUTTON0PIN 2
#define BUTTON1PIN 3
#define LEDPIN 13

#define TIMER_FREQUENCY 1000
#define DEBOUNCE_INTERVAL 20
#define WAIT_INTERVAL 100

class Timer
{
public:
  static const unsigned int frequency = TIMER_FREQUENCY;
  static const unsigned long period = (long)(1e6 / frequency);
  static const unsigned long periodToMs = period / 1000;

  void configure(void (*tick)())
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
  void start() { Timer4.start(); }
  void stop() { Timer4.stop(); }
};
class LED
{
public:
  enum class State
  {
    off,
    on,
    blinking
  };

  State state = State::off;

  LED(int pin) : pin(pin) { pinMode(pin, OUTPUT); }

  void update(State state)
  {
    this->state = state;
    if (state != State::blinking)
      digitalWrite(pin, state == State::on ? HIGH : LOW);
  }

  void blink()
  {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
  }

private:
  const int pin;
};
class Button
{
public:
  enum State
  {
    released,
    active,
    pressed,
    idle
  };

  State state = released;
  void (*press)();

  Button(int pin, void (*press)()) : pin(pin), press(press)
  {
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin), startTimer, CHANGE);
  }

  bool isDebouncing() { return debounceTime > -1; }
  bool isWaiting() { return waitTime > -1; }

  void update()
  {
    switch (state)
    {
    case released: // button is released
      if (read() == LOW)
      {
        state = active;
        debounceTime = 0; // start debouncing
      }
      break;
    case active: // low signal detected
      if (read() == HIGH)
        state = released;
      break;
    case pressed: // button is pressed
      if (read() == HIGH)
      {
        state = idle;
        debounceTime = 0; // start debouncing
      }
      break;
    case idle: // high signal detected
      if (read() == LOW)
        state = pressed;
    }

    if (isDebouncing())
      debounce();
    else if (isWaiting())
      wait();
  };

  void cancel()
  {
    noInterrupts();
    waitTime = -1;
    timer.stop();
    interrupts();
  }

private:
  static const unsigned int debounceInterval = DEBOUNCE_INTERVAL, waitInterval = WAIT_INTERVAL;

  static Timer timer;
  static void startTimer() { timer.start(); }

  const unsigned int pin;
  long debounceTime = -1; // ms (-1: not debouncing)
  long waitTime = -1;     // ms (-1: not waiting)

  bool read() { return digitalRead(pin); }
  void debounce()
  {
    debounceTime += Timer::periodToMs;

    if (debounceTime > debounceInterval)
    {
      if (state == active)
        state = pressed;
      else if (state == idle)
      {
        state = released;
        waitTime = 0; // start waiting
      }

      debounceTime = -1;
    }
  }
  void wait()
  {
    waitTime += Timer::periodToMs;

    if (waitTime > waitInterval)
    {
      press();
      cancel();
    }
  }
};

static Timer timer = Timer();
static LED led = LED(LEDPIN);

void press1()
{
  led.update(LED::State::on);
  Serial.println("Button 1 pressed.");
}
void press2()
{
  led.update(LED::State::off);
  Serial.println("Button 2 pressed.");
}
void pressBoth()
{
  led.update(LED::State::blinking);
  Serial.println("Both buttons pressed.");
}

static Button button[2] = {
    Button(BUTTON0PIN, press1),
    Button(BUTTON1PIN, press2)};

static void tick()
{
  if (button[0].isWaiting() && button[1].isWaiting())
  {
    pressBoth();
    for (auto &b : button)
      b.cancel(); // cancel the individual button actions
  }
  else
    for (auto &b : button)
      b.update();
}

void setup34()
{
  timer.configure(tick);

  Serial.begin(9600);

  Serial.println("Press buttons to change LED state:");
  Serial.println("  Button 1: LED on");
  Serial.println("  Button 2: LED off");
  Serial.println("  Both buttons: LED blinking");
}

void loop34()
{
  if (led.state == LED::State::blinking)
    led.blink();
}