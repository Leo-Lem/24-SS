#include <SPI.h>
#include "DisplayBuffer.h"
#include "Window.h"

#ifndef DISPLAY_H
#define DISPLAY_H

class Display
{
public:
  enum Command : uint8_t
  {
    NOP = 0x00,     // no Operation
    SWRESET = 0x01, // Software reset
    SLPOUT = 0x11,  // Sleep out & booster on
    DISPOFF = 0x28, // Display off
    DISPON = 0x29,  // Display on
    CASET = 0x2A,   // Column adress set
    RASET = 0x2B,   // Row adress set
    RAMWR = 0x2C,   // Memory write
    MADCTL = 0x36,  // Memory Data Access Control
    COLMOD = 0x3A,  // RGB-format 12/16/18bit
    INVOFF = 0x20,  // Display inversion off
    INVON = 0x21,   // Display inversion on
    INVCTR = 0xB4,  // Display Inversion mode control
    NORON = 0x13,   // Partial off (Normal)
    PWCRT1 = 0xC0,  // Power Control 1
    PWCRT2 = 0xC1,  // Power Control 2
    PWCRT3 = 0xC2,  // Power Control 3
    PWCRT4 = 0xC3,  // Power Control 4
    PWCRT5 = 0xC4,  // Power Control 5
    VMCTR1 = 0xC5   // VCOM Voltage setting
  };

  Window window;

  Display(int width, int height) : window(Window(width, height)),
                                   buffer(DisplayBuffer<COLOR_FORMAT>())
  {
    pinMode(RST, OUTPUT);
    pinMode(DC, OUTPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(RST, HIGH);
    digitalWrite(DC, HIGH);
    digitalWrite(CS, HIGH);
  };

  void init()
  {
    SPI.begin();
    delay(10);

#ifdef DEBUG
    Serial.print("[SPI] Initialized with ");
    Serial.print(SPI_DEFAULT_FREQ, DEC);
    Serial.println("Hz.");

    int time = millis();
#endif

    // hardware reset
    digitalWrite(RST, LOW);
    delay(100);
    digitalWrite(RST, HIGH);
    delay(100);

    beginTransaction();

    // software reset, turn off sleep mode, power control settings, VCOM voltage setting
    writeCommand(SWRESET);
    delay(120);           // mandatory delay
    writeCommand(SLPOUT); // turn off sleep mode.
    delay(120);
    writeCommand(PWCRT1);
    writeData(3, 0xA2, 0x02, 0x84);
    writeCommand(PWCRT4);
    writeData(2, 0x8A, 0x2A);
    writeCommand(PWCRT5);
    writeData(2, 0x8A, 0xEE);
    writeCommand(VMCTR1);
    writeData(1, 0x0E); // VCOM = -0.775V

    // Memory Data Access Control D7/D6/D5/D4/D3/D2/D1/D0
    //  MY/MX/MV/ML/RGB/MH/-/-
    //  MY- Row Address Order; ‘0’ =Increment, (Top to Bottom)
    //  MX- Column Address Order; ‘0’ =Increment, (Left to Right)
    //  MV- Row/Column Exchange; '0’ = Normal,
    //  ML- Scan Address Order; ‘0’ =Decrement,(LCD refresh Top to Bottom)
    //  RGB - '0'= RGB color fill order
    //  MH - '0'= LCD horizontal refresh left to right

    writeCommand(MADCTL);
    writeData(1, 0xC8);

    // RGB-format
    writeCommand(COLMOD);
    writeData(1, 0x05); // 16-bit/pixel; high nibble don't care

    configureArea(window);

    writeCommand(NORON);
    writeCommand(DISPON);

    endTransaction();

#ifdef DEBUG
    time = millis() - time;
    Serial.print("[Display] Initialized in ");
    Serial.print(time, DEC);
    Serial.println("ms.");
#endif

    // initialise buffer
    configureArea(window);
  };

  void configureArea(Window area)
  {
    beginTransaction();
    buffer.init(area);
    writeCommand(CASET);
    writeData(4, 0x00, area.xs(), 0x00, area.xe());
    writeCommand(RASET);
    writeData(4, 0x00, area.ys(), 0x00, area.ye());
    endTransaction();

#ifdef DEBUG
    Serial.print("[Display] Configured drawing area x(");
    Serial.print(area.xs());
    Serial.print(" to ");
    Serial.print(area.xe());
    Serial.print("), y(");
    Serial.print(area.ys());
    Serial.print(" to ");
    Serial.print(area.ye());
    Serial.println(").");
#endif
  };
  void setPixel(int x, int y, Color color)
  {
    buffer.set(x, y, color);

#ifdef DEBUG
    Serial.print("[Display] Pixel in buffer at ");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(" set to color #");
    Serial.print((uint16_t)color, HEX);
    Serial.println(" (RGB565).");
#endif
  };
  void drawPixels()
  {
    beginTransaction();
    writeCommand(RAMWR);
    COLOR_FORMAT *colors = buffer.getBuffer();

#ifdef DEBUG
    Serial.print("[Display] Buffer content: ");
    for (int i = 0; i < buffer.size(); i++)
    {
      Serial.print((uint16_t)colors[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
#endif

    for (int i = 0; i < buffer.size(); i++)
    {
      uint16_t encoded = Color(colors[i]);
      writeData(2, encoded >> 8, encoded & 0xFF);
    }
    endTransaction();

#ifdef DEBUG
    Serial.print("[Display] Drawn ");
    Serial.print(buffer.size());
    Serial.print(" pixels in ");
    Serial.print(buffer.getArea().width);
    Serial.print("x");
    Serial.print(buffer.getArea().height);
    Serial.println(" window.");
#endif
  };

  void fill(Color color)
  {
#ifdef DEBUG
    Serial.print("[Display] Filling with #");
    Serial.print((uint16_t)color, HEX);
    Serial.println(" (RGB565).");
#endif
    buffer.fill(color);
  };
  void invert(bool on)
  {
    beginTransaction();
    writeCommand(on ? INVON : INVOFF);
    endTransaction();
  };

private:
  const static int RST = 9, DC = 8;

#ifdef __AVR_ATmega2560__
  const static int CS = 53;
  typedef uint8_t COLOR_FORMAT;
#else
  const static int CS = 10;
  typedef uint16_t COLOR_FORMAT;
#endif

  const static int SPI_DEFAULT_FREQ = 1e6;
  SPISettings settings = SPISettings(SPI_DEFAULT_FREQ, MSBFIRST, SPI_MODE0);

  DisplayBuffer<COLOR_FORMAT> buffer;

  void commandMode() { digitalWrite(DC, LOW); }
  void dataMode() { digitalWrite(DC, HIGH); }
  void on() { digitalWrite(CS, LOW); }
  void off() { digitalWrite(CS, HIGH); }
  void beginTransaction()
  {
    SPI.beginTransaction(settings);
    on();
  };
  void endTransaction()
  {
    off();
    SPI.endTransaction();
  };

  void writeCommand(Command cmd)
  {
    commandMode();
    SPI.transfer(cmd);
    dataMode();
  };
  void writeData(size_t count, ...)
  {
    va_list args;
    va_start(args, count);

    for (size_t i = 0; i < count; ++i)
      SPI.transfer(va_arg(args, int));

    va_end(args);
  }
};

#endif // DISPLAY_H