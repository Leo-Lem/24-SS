#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

void demo()
{
  Window area = display.getDimensions();

  display.fill(Color::white);                      // fill display with white
  display.fill(Color::blue, Window(area, 20, 20)); // draw centered blue rectangle

  // draw centered red colored cross with two pixel width
  const int yc = area.ys() + area.height / 2;
  display.configureBuffer(Window(area.xs(), yc - 1, area.xe(), yc + 1));
  for (int x = 0; x < area.width; x++)
    display.setPixel(x, 0, Color::red);

  display.drawBuffer();

  const int xc = area.xs() + area.width / 2;
  display.configureBuffer(Window(xc - 1, area.ys(), xc + 1, area.ye()));
  for (int y = 0; y < area.height; y++)
    display.setPixel(0, y, Color::red);

  display.drawBuffer();
};

void setup()
{
  Serial.begin(9600); // open serial port
  Serial.println("[Demo] TFT-LCD demo setup…");

  display.init();

  demo(); // draw the demo image
  Serial.println("[Demo] Setup done.");

  Serial.println("[Demo] TFT-LCD demo starting…");
}

bool isInverted = false;

void loop()
{
  delay(1000);
  display.invert(isInverted = !isInverted);
}
