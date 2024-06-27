
// TODO: fix buffer not clearing on configure

#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

void activity(int length, int delayMS = 1000)
{
  Window area(display.getDimensions(), length, length);

  const int
      xc = area.xs() + area.width / 2,
      yc = area.ys() + area.height / 2,
      radius = length / 2;

  // symbol |
  display.clear(area);
  display.fill(Color::white, Window(xc, yc - radius, xc + 1, yc + radius));
  Serial.print("[Exercise 6.4] Symbol | drawn.\r");
  delay(delayMS);

  // symbol /
  display.clear(area);
  display.configureBuffer(area);
  for (int i = 0; i < length; i++)
    display.setPixel(area.width - i, i, Color::white);
  display.drawBuffer();

  Serial.print("[Exercise 6.4] Symbol / drawn.\r");
  delay(delayMS);

  // symbol -
  display.clear(area);
  display.fill(Color::white, Window(xc - radius, yc, xc + radius, yc + 1));

  Serial.print("[Exercise 6.4] Symbol - drawn.\r");
  delay(delayMS);

  // symbol "\"
  display.clear(area);
  display.configureBuffer(area);
  for (int i = 0; i < length; i++)
    display.setPixel(i, i, Color::white);
  display.drawBuffer();

  Serial.print("[Exercise 6.4] Symbol \\ drawn.\r");
  delay(delayMS);
}

void setup()
{
  Serial.begin(9600);
  display.init();

  Serial.println("[Exercise 6.4] Setup done.");
}

void loop() { activity(50, 1000); }