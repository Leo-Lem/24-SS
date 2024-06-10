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
  static void configure(void (*tick)(), unsigned int period = 1)
  {
#if defined(__SAM3X8E__) // Arduino Due
    if (!Timer4.configure(1 / period, tick))
      Serial.println("Failed to start timer…");
#else
    Timer4.initialize(period * 1e3);
    Timer4.attachInterrupt(tick);
    Timer4.stop(); // attachInterrupt starts Timer > stop it
#endif
  }

  static void start() { Timer4.start(); }
  static void stop() { Timer4.stop(); }
};

#endif