#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

const Color fgColor = Color::yellow, bgColor = Color::black;

void clear()
{
  display.fill(bgColor);
  Serial.println("[Exercise 6.3] Cleared Display.");
}

void columns()
{
  for (int x = display.getDimensions().xs(); x < display.getDimensions().xe(); x++)
  {
    display.configureBuffer(Window(x, display.getDimensions().ys(), x + 1, display.getDimensions().ye()));
    for (int y = 0; y < display.getDimensions().height; y++)
      display.setPixel(x, y, fgColor);

    display.drawBuffer();
    delay(20);
  }

  Serial.println("[Exercise 6.3] Filled Display with fgColor.");

  for (int x = display.getDimensions().xs(); x < display.getDimensions().xe(); x++)
  {
    display.configureBuffer(Window(x, display.getDimensions().ys(), x + 1, display.getDimensions().ye()));
    for (int y = 0; y < display.getDimensions().height; y++)
      display.setPixel(x, y, bgColor);

    display.drawBuffer();
    delay(20);
  }

  Serial.println("[Exercise 6.3] Cleared Display with bgColor.");
}

void setup()
{
  Serial.begin(9600);
  display.init();
}

void loop() { columns(); }