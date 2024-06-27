

#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

void printChar(char c)
{
  int result = display.printChar(
      display.getDimensions().width / 2,
      display.getDimensions().height - 10,
      c, Color::white, Color::black);

  Serial.print("[Exercise 6.5] Character ");
  Serial.print(c);
  Serial.println((result == -1) ? " not printed." : " printed.");
}

void printString(char *str)
{
  int result = display.printString(
      display.getDimensions().width / 2 - strlen(str) * 6 / 2,
      display.getDimensions().height / 2,
      str, Color::green, Color::black);

  Serial.print("[Exercise 6.5] String ");
  Serial.print(str);
  Serial.println((result == -1) ? " not printed." : " printed.");
}

void setup()
{
  Serial.begin(9600);
  display.init();
  Serial.println("[Exercise 6.5] Setup done.");

  display.clear();

  printChar('A');
  printString("Hello, world!");
}

void loop() {}