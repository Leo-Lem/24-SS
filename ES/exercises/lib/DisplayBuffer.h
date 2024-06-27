

#include <stdint.h>
#include "Color.h"
#include "Window.h"

#ifndef DISPLAYBUFFER_H
#define DISPLAYBUFFER_H

#if __AVR_ATmega2560__
#define MAX_MEMORY_B 7600
#else
#define MAX_MEMORY_B 65000
#endif

template <typename BufferFormat>
class DisplayBuffer
{
public:
  DisplayBuffer() {}

  void init(Window area)
  {
    if (buffer != nullptr)
      delete[] buffer;
    this->area = area;
    buffer = new BufferFormat[size()];

#ifdef DEBUG
    Serial.print("[DisplayBuffer] ");
    Serial.print(size());
    Serial.print(" bytes allocated for ");
    Serial.print(area.width);
    Serial.print("x");
    Serial.print(area.height);
    Serial.print(" window (");
    Serial.print(isTruncated() ? "truncated" : "full");
    Serial.println(" memory usage).");
#endif
  }

  BufferFormat *getBuffer() { return buffer; }
  Window getArea() { return area; }

  void set(int x, int y, Color color)
  {
    if (x >= 0 && x < area.width && y >= 0 && y < area.height && x + y * area.width < size())
      buffer[y * area.width + x] = (BufferFormat)color;
    else
    {
#ifdef DEBUG
      Serial.print("[DisplayBuffer] Pixel at ");
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.println(" out of bounds.");
#endif
    }
  }

  int size() { return min(area.size() * sizeof(BufferFormat), MAX_MEMORY_B / sizeof(BufferFormat)); }
  bool isTruncated() { return area.size() > MAX_MEMORY_B / sizeof(BufferFormat); }

private:
  Window area = Window(0, 0);
  BufferFormat *buffer;
};

#endif // DISPLAYBUFFER_H