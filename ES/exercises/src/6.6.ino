#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display = Display::limitedMemory;
#else
Display display = Display::st7735;
#endif

char *names[] = {"Fritz", "Leopold"},
     *matriculationNumbers[] = {"0000000", "7724741"};

void demo()
{
  const int xc = display.window.width / 2, yc = display.window.height / 2;

  for (int i = 0; i < 2; i++)
  {
    display.clear();
    display.printString(xc - strlen(names[i]) * 6 / 2, yc + 2 + 8, names[i], Color::white, Color::black);
    display.printString(xc - strlen(matriculationNumbers[i]) * 6 / 2, yc - 3, matriculationNumbers[i], Color::white, Color::black);
    delay(5000);
  }
}

void setup()
{
  Serial.begin(9600); // open serial port
  display.init();     // power-on-reset of Display
  Serial.println("[Exercise 6.6] Setup done.");
  Serial.println("[Exercise 6.6] Starting student id demo.");
}

void loop() { demo(); }