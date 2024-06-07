// #define DEBUG
#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display(240, 640); // 2.2" TFT, 240x620 pixel
#else
Display display(160, 128); // 1.8" TFT, 160x128 pixel
#endif

const Color fgColor = Color::yellow, bgColor = Color::black;

void clear()
{
#ifdef __AVR_ATmega2560__
  // set up smaller area due to memory constraints
  Window area(70, 70);
  display.configureArea(area);
#endif

  // clear display
  display.fill(bgColor);
  Serial.println("[Exercise 6.2] Cleared Display.");
}

void columns()
{
  for (int x = display.window.xs(); x < display.window.xe(); x++)
  {
    display.configureArea(Window(x, x + 1, display.window.ys(), display.window.ye()));
    for (int y = 0; y < display.window.height; y++)
      display.setPixel(x, y, fgColor);

    display.drawPixels();
    delay(20);
  }

  Serial.println("[Exercise 6.2] Filled Display with fgColor.");

  for (int x = display.window.xs(); x < display.window.xe(); x++)
  {
    display.configureArea(Window(x, x + 1, display.window.ys(), display.window.ye()));
    for (int y = 0; y < display.window.height; y++)
      display.setPixel(x, y, bgColor);

    display.drawPixels();
    delay(20);
  }

  Serial.println("[Exercise 6.2] Cleared Display with bgColor.");
}

void setup()
{
  Serial.begin(9600); // open serial port
  display.init();     // power-on-reset of Display
}

void loop() { columns(); }