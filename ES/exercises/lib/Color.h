

#include <stdint.h>

#ifndef COLOR_H
#define COLOR_H

struct Color
{
  uint8_t r, g, b;

  // Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
  Color(uint16_t rgb565)
      : r((rgb565 >> 8) & 0xF8 | (rgb565 >> 13) & 0x07),
        g((rgb565 >> 3) & 0xFC | (rgb565 >> 9) & 0x03),
        b((rgb565 << 3) & 0xF8 | (rgb565 >> 2) & 0x07) {}

  Color(uint8_t rgb332)
      : r((rgb332 & 0xE0) | ((rgb332 & 0xE0) >> 3) | ((rgb332 & 0xE0) >> 6)),
        g(((rgb332 & 0x1C) << 3) | ((rgb332 & 0x1C) << 1) | ((rgb332 & 0x1C) >> 1)),
        b((rgb332 & 0x03) * 85) {} // 85 = 255 / 3 for 2-bit to 8-bit conversion

  operator uint16_t() const { return (r & 0xF8) << 8 | (g & 0xFC) << 3 | (b & 0xF8) >> 3; }
  operator uint8_t() const { return (r & 0xE0) | (g & 0xE0) >> 3 | (b & 0xC0) >> 6; }

  static const Color black, white, red, green, blue, yellow;
};

const Color Color::black = Color((uint16_t)0x0000),
            Color::white = Color((uint16_t)0xFFFF),
            Color::red = Color((uint16_t)0xF800),
            Color::green = Color((uint16_t)0x07E0),
            Color::blue = Color((uint16_t)0x001F),
            Color::yellow = Color((uint16_t)0xFFE0);

#endif // COLOR_H