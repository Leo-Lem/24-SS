#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display = Display::limitedMemory;
#else
Display display = Display::st7735;
#endif

void printChar(char c)
{
  int result = display.printChar(10, 10, c, Color::white, Color::black);

  Serial.print("[Exercise 6.5] Character ");
  Serial.print(c);
  Serial.println((result == -1) ? " not printed." : " printed.");
}

void printString(char *str)
{
  int result = display.printString(30, 20, str, Color::green, Color::black);

  Serial.print("[Exercise 6.5] String ");
  Serial.print(str);
  Serial.println((result == -1) ? " not printed." : " printed.");
}

void setup()
{
  Serial.begin(9600); // open serial port
  display.init();     // power-on-reset of Display
  Serial.println("[Exercise 6.5] Setup done.");

  display.clear();

  printChar('A');
  printString("Hi!");
}

void loop() {}