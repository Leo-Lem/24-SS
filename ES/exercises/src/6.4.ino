#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display = Display::limitedMemory;
#else
Display display = Display::st7735;
#endif

void activity(int length, int delayMS = 1000 / 10)
{
  const int
      xc = display.window.width / 2,
      yc = display.window.height / 2,
      radius = length / 2;

  // symbol |
  display.clear();
  display.configureArea(Window(xc, xc + 1, yc - radius, yc + radius));
  display.fill(Color::white);
  display.drawPixels();

  Serial.print("[Exercise 6.4] Symbol | drawn.\r");
  delay(delayMS);

  // symbol /
  display.clear();
  display.configureArea(Window(xc - radius, xc + radius, yc - radius, yc + radius));
  for (int i = 0; i < length; i++)
    display.setPixel(xc - radius + i, yc + radius - i, Color::white);
  display.drawPixels();

  Serial.print("[Exercise 6.4] Symbol / drawn.\r");
  delay(delayMS);

  // symbol -
  display.clear();
  display.configureArea(Window(xc - radius, xc + radius, yc, yc + 1));
  display.fill(Color::white);
  display.drawPixels();

  Serial.print("[Exercise 6.4] Symbol - drawn.\r");
  delay(delayMS);

  // symbol "\"
  display.clear();
  display.configureArea(Window(xc - radius, xc + radius, yc - radius, yc + radius));
  for (int i = 0; i < length; i++)
    display.setPixel(xc - radius + i, yc - radius + i, Color::white);
  display.drawPixels();

  Serial.print("[Exercise 6.4] Symbol \\ drawn.\r");
  delay(delayMS);
}

void setup()
{
  Serial.begin(9600); // open serial port
  display.init();     // power-on-reset of Display

  Serial.println("[Exercise 6.4] Setup done. Starting activity indicator.");
}

void loop()
{
  activity(50, 1000 / 5);
}