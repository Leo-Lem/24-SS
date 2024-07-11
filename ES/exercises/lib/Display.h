#include <SPI.h>
#include "DisplayBuffer.h"
#include "Font_6x8.h"
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

  Display(int width, int height) : dimensions(Window(width, height)),
                                   buffer(DisplayBuffer<COLOR_FORMAT>()){};

  void init()
  {
    pinMode(RST, OUTPUT);
    pinMode(DC, OUTPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(RST, HIGH);
    digitalWrite(DC, HIGH);
    digitalWrite(CS, HIGH);

#ifdef __AVR_ATmega2560__
    pinMode(SS_SLAVE, OUTPUT); // otherwise the Mega will go into slave mode.
#endif

    SPI.begin();
    delay(100);

#ifdef DEBUG
    Serial.print("[SPI] Initialized.");

    int time = millis();
#endif

    // hardware reset
    digitalWrite(RST, HIGH);
    delay(100);
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
    writeData(0xA2);
    writeData(0x02);
    writeData(0x84);
    writeCommand(PWCRT4);
    writeData(0x8A);
    writeData(0x2A);
    writeCommand(PWCRT5);
    writeData(0x8A);
    writeData(0xEE);
    writeCommand(VMCTR1);
    writeData(0x0E); // VCOM = -0.775V

    // Memory Data Access Control D7/D6/D5/D4/D3/D2/D1/D0
    //  MY/MX/MV/ML/RGB/MH/-/-
    //  MY- Row Address Order; ‘0’ =Increment, (Top to Bottom)
    //  MX- Column Address Order; ‘0’ =Increment, (Left to Right)
    //  MV- Row/Column Exchange; '0’ = Normal,
    //  ML- Scan Address Order; ‘0’ =Decrement,(LCD refresh Top to Bottom)
    //  RGB - '0'= RGB color fill order
    //  MH - '0'= LCD horizontal refresh left to right

    writeCommand(MADCTL);
    writeData(0xC0);

    // RGB-format
    writeCommand(COLMOD);
    writeData(0x05); // 16-bit/pixel; high nibble don't care

    configureDrawArea(dimensions);
    configureBuffer(dimensions);

    writeCommand(NORON);
    writeCommand(DISPON);

    endTransaction();

#ifdef DEBUG
    time = millis() - time;
    Serial.print("[Display] Initialized in ");
    Serial.print(time, DEC);
    Serial.println("ms.");
#endif
  };

  Window getDimensions() { return dimensions; }

  // using the buffer
  void configureBuffer(Window area)
  {
    buffer.init(area);

#ifdef DEBUG
    Serial.print("[Display] Buffer configured for ");
    Serial.print(area.width);
    Serial.print("x");
    Serial.print(area.height);
    Serial.println(" window.");
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
  void drawBuffer()
  {
    configureDrawArea(buffer.getArea());

    beginTransaction();
    writeCommand(RAMWR);
    COLOR_FORMAT *colors = buffer.getBuffer();

    for (int i = 0; i < buffer.size(); i++)
      writeColor((uint16_t)colors[i]);

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
  void scaleAndDraw()
  {
    beginTransaction();
    writeCommand(RAMWR);
    COLOR_FORMAT *colors = buffer.getBuffer();

    int xScale = dimensions.width / buffer.getArea().width,
        yScale = dimensions.height / buffer.getArea().height;

    configureDrawArea(dimensions);

    for (int y = 0; y < buffer.getArea().height; y++)
      for (int x = 0; x < buffer.getArea().width; x++)
        for (int i = 0; i < xScale; i++)
          for (int j = 0; j < yScale; j++)
            writeColor((uint16_t)colors[y * buffer.getArea().width + x]);

    endTransaction();

#ifdef DEBUG
    Serial.print("[Display] Scaled and drew");
    Serial.print(buffer.size());
    Serial.print(" pixels in ");
    Serial.print(dimensions.width);
    Serial.print("x");
    Serial.print(dimensions.height);
    Serial.println(" window.");
#endif
  };

  // fill the display directly, without using the buffer
  void fill(Color color) { fill(color, dimensions); };
  void clear() { fill(Color::black); };
  void clear(Window area) { fill(Color::black, area); };
  void fill(Color color, Window area)
  {
    configureDrawArea(area);

    beginTransaction();
    writeCommand(RAMWR);

    for (int i = 0; i < area.size(); i++)
      writeColor((uint16_t)color);

    endTransaction();

#ifdef DEBUG
    Serial.print("[Display] Filled ");
    Serial.print(area.width);
    Serial.print("x");
    Serial.print(area.height);
    Serial.print(" window with #");
    Serial.print((uint16_t)color, HEX);
    Serial.println(" (RGB565).");
#endif
  };

  void invert(bool on)
  {
    beginTransaction();
    writeCommand(on ? INVON : INVOFF);
    endTransaction();

#ifdef DEBUG
    Serial.print("[Display] Color inversion is ");
    Serial.println(on ? "on." : "off.");
#endif
  };

  // print text
  int printChar(int x, int y, char value, Color fgColor, Color bgColor)
  {
    Window area(x, y, x + CHAR_WIDTH, y + CHAR_HEIGHT);

    if (area.width > dimensions.width || area.height > dimensions.height)
      return -1;

    configureBuffer(area);
    configureDrawArea(area);

    for (int i = 0; i < CHAR_WIDTH; i++)
      for (int j = 0; j < CHAR_HEIGHT; j++)
        if ((font[value - ' '][i]) & (1 << (CHAR_HEIGHT - 1 - j)))
          setPixel(i, j, fgColor);
        else
          setPixel(i, j, bgColor);

    drawBuffer();

    return 0;

#ifdef DEBUG
    Serial.print("[Display] Character '");
    Serial.print(value);
    Serial.print("' printed at ");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(" with fg #");
    Serial.print((uint16_t)fgColor, HEX);
    Serial.print(" and bg #");
    Serial.println((uint16_t)bgColor, HEX);
#endif
  }

  int printString(int x, int y, char *c_str, Color fgColor, Color bgColor)
  {
    if (x + strlen(c_str) * 6 > dimensions.width || y + 8 > dimensions.height)
      return -1;
    else
      for (int i = 0; c_str[i] != '\0'; i++)
        printChar(x + i * 6, y, c_str[i], fgColor, bgColor);

    return 0;
  }

private:
  const static int RST = 9, DC = 8, CS = 10;

  const static int CHAR_WIDTH = 6, CHAR_HEIGHT = 8;

#ifdef __AVR_ATmega2560__
  const static int SS_SLAVE = 53;
  typedef uint8_t COLOR_FORMAT;
#else
  typedef uint16_t COLOR_FORMAT;
#endif

  SPISettings settings = SPISettings(40e6, MSBFIRST, SPI_MODE0);

  Window dimensions;
  DisplayBuffer<COLOR_FORMAT> buffer;

  void configureDrawArea(Window area)
  {
    beginTransaction();
    writeCommand(NOP);
    writeCommand(CASET);
    writeColor(area.xs()), writeColor(area.xe() - 1);
    writeCommand(RASET);
    writeColor(area.ys()), writeColor(area.ye() - 1);
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

  void commandMode() { digitalWrite(DC, LOW); }
  void dataMode() { digitalWrite(DC, HIGH); }
  void on() { digitalWrite(CS, LOW); }
  void off() { digitalWrite(CS, HIGH); }

  void beginTransaction()
  {
    SPI.beginTransaction(settings);
    on();
  }
  void endTransaction()
  {
    off();
    SPI.endTransaction();
  }

  void writeCommand(Command cmd)
  {
    commandMode();
    SPI.transfer(cmd);
    dataMode();
  }
  void writeData(uint8_t data) { SPI.transfer(data); }
  void writeColor(uint16_t color) { SPI.transfer16(color); }

public:
  const static Display ili9341, st7735;
};

const Display
    Display::ili9341 = Display(240, 320), // 2.2" TFT, 320 pixel, black part below is not display !!!
    Display::st7735 = Display(128, 160);  // 1.8" TFT, 160x128 pixel

#endif // DISPLAY_H