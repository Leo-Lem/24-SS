#ifndef TIMER_H
#define TIMER_H

#if defined(__SAM3X8E__) // Arduino Due
#include "DueTimer.h"
static DueTimer Timer4; // match TimerFour package
#else
#include <TimerFour.h>
#endif

class Timer
{
public:
  static const unsigned int frequency = 1000;

  static const unsigned long period = (long)(1e6 / frequency);
  static const unsigned long periodToMs = period / 1000;

  static void configure(void (*tick)())
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
  static void start() { Timer4.start(); }
  static void stop() { Timer4.stop(); }
};

#endif