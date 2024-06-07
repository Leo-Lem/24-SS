#include <Arduino.h>

#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display(240, 320); // 2.2" TFT, 240x320 pixel (uint8 max = 255)
#else
Display display(160, 128); // 1.8" TFT, 160x128 pixel
#endif

const Color fgColor = Color::yellow, bgColor = Color::black;

void setup()
{
  Serial.begin(9600); // open serial port
  Serial.println("TFT-LCD demo starting…");

  SPI.begin(); // initialize SPI
  Serial.println("Initialized SPI.");
  delay(10);

  display.init(); // power-on-reset of Display
  Serial.println("Initialized Display.");
  delay(100);

  // clear display
  display.fill(bgColor);
  Serial.println("Cleared Display.");

  while (true)
  {
    for (int y = display.window.ys(); y < display.window.ye(); y++)
    {
      for (int x = display.window.xs(); x < display.window.xe(); x++)
        display.setPixel(x, y, fgColor);
      display.drawPixels();
      delay(20);
    }

    Serial.println("Filled Display with fgColor.");

    for (int i = display.window.xe(); i > display.window.xs(); i--)
    {
      for (int j = display.window.ye(); j > display.window.ys(); j--)
        display.setPixel(i, j, bgColor);
      display.drawPixels();
      delay(20);
    }

    Serial.println("Cleared Display with bgColor.");
  }
}

void loop() {}