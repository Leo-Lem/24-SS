// ES-exercise 06                                                                   //
// Demo to initialize TFT-Display with ST7735R controller,          //
// e.g. joy-it RB-TFT1.8-V2. 						                            //
// configuration:  4-line serial interface, RGB-order: R-G-B,        //
#include <SPI.h>
#include <stdint.h>

#define TFT_CS 10 // display: CS-pin
#define TFT_RST 9 // display: reset
#define TFT_DC 8  // display: Data/Command (D/C)

#define TFT_DC_HIGH() digitalWrite(TFT_DC, HIGH)
#define TFT_DC_LOW() digitalWrite(TFT_DC, LOW)
#define TFT_CS_HIGH() digitalWrite(TFT_CS, HIGH)
#define TFT_CS_LOW() digitalWrite(TFT_CS, LOW)

// TFT's commands
const uint8_t NOP = 0x00;     // no Operation
const uint8_t SWRESET = 0x01; // Software reset
const uint8_t SLPOUT = 0x11;  // Sleep out & booster on
const uint8_t DISPOFF = 0x28; // Display off
const uint8_t DISPON = 0x29;  // Display on
const uint8_t CASET = 0x2A;   // Column adress set
const uint8_t RASET = 0x2B;   // Row adress set
const uint8_t RAMWR = 0x2C;   // Memory write
const uint8_t MADCTL = 0x36;  // Memory Data Access Control
const uint8_t COLMOD = 0x3A;  // RGB-format, 12/16/18bit
const uint8_t INVOFF = 0x20;  // Display inversion off
const uint8_t INVON = 0x21;   // Display inversion on
const uint8_t INVCTR = 0xB4;  // Display Inversion mode control
const uint8_t NORON = 0x13;   // Partial off (Normal)

const uint8_t PWCTR1 = 0xC0; // Power Control 1
const uint8_t PWCTR2 = 0xC1; // Power Control 2
const uint8_t PWCTR3 = 0xC2; // Power Control 3
const uint8_t PWCTR4 = 0xC3; // Power Control 4
const uint8_t PWCTR5 = 0xC4; // Power Control 5
const uint8_t VMCTR1 = 0xC5; // VCOM Voltage setting

#ifndef TIMER_H
#define TIMER_H

#if defined(__SAM3X8E__) // Arduino Due
#include "DueTimer.h"
static DueTimer Timer4; // match TimerFour package
#else
#include <TimerFour.h>
#endif

class Timer
{
public:
  static void configure(void (*tick)(), unsigned int period = 1)
  {
#if defined(__SAM3X8E__) // Arduino Due
    if (!Timer4.configure(1 / period, tick))
      Serial.println("Failed to start timer…");
#else

    Serial.println("[Exercise 6.4] Conf Timer.");
    delay(1000);
    Timer4.initialize(period * 1e3);
    Timer4.attachInterrupt(tick);
    Timer4.stop(); // attachInterrupt starts Timer > stop it
#endif
  }

  static void start() { Timer4.start(); }
  static void stop() { Timer4.stop(); }
};

#endif
#if defined(__AVR_ATmega2560__)
#define SS_SLAVE 53
#endif
// #define DEBUG
struct Window
{
public:
  int x, y, width, height;

  Window(int width, int height) : x(0), y(0), width(width), height(height) {}
  Window(int xs, int xe, int ys, int ye) : x(xs), y(ys), width(xe - xs), height(ye - ys) {}

  int xs()
  {
    return int(x);
  }
  int xe()
  {
    return int(x) + int(width);
  }
  int ys() { return y; }
  int ye() { return y + height; }

  int size() { return width * height; }
};

#if __AVR_ATmega2560__
#define MAX_MEMORY_B 6100
#else
#define MAX_MEMORY_B 65000
#endif

unsigned char font[95][6] =
    {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // space
        {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00}, // !
        {0x00, 0x07, 0x00, 0x07, 0x00, 0x00}, // "
        {0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00}, // #
        {0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00}, // $
        {0x23, 0x13, 0x08, 0x64, 0x62, 0x00}, // %
        {0x36, 0x49, 0x55, 0x22, 0x50, 0x00}, // &
        {0x00, 0x00, 0x07, 0x00, 0x00, 0x00}, // '
        {0x00, 0x1C, 0x22, 0x41, 0x00, 0x00}, // (
        {0x00, 0x41, 0x22, 0x1C, 0x00, 0x00}, // )
        {0x0A, 0x04, 0x1F, 0x04, 0x0A, 0x00}, // *
        {0x08, 0x08, 0x3E, 0x08, 0x08, 0x00}, // +
        {0x00, 0x50, 0x30, 0x00, 0x00, 0x00}, // ,
        {0x08, 0x08, 0x08, 0x08, 0x08, 0x00}, // -
        {0x00, 0x60, 0x60, 0x00, 0x00, 0x00}, // .
        {0x20, 0x10, 0x08, 0x04, 0x02, 0x00}, // slash
        {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00}, // 0
        {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00}, // 1
        {0x42, 0x61, 0x51, 0x49, 0x46, 0x00}, // 2
        {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00}, // 3
        {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00}, // 4
        {0x27, 0x45, 0x45, 0x45, 0x39, 0x00}, // 5
        {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00}, // 6
        {0x03, 0x71, 0x09, 0x05, 0x03, 0x00}, // 7
        {0x36, 0x49, 0x49, 0x49, 0x36, 0x00}, // 8
        {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00}, // 9
        {0x00, 0x36, 0x36, 0x00, 0x00, 0x00}, // :
        {0x00, 0x56, 0x36, 0x00, 0x00, 0x00}, // ;
        {0x08, 0x14, 0x22, 0x41, 0x00, 0x00}, // <
        {0x14, 0x14, 0x14, 0x14, 0x14, 0x00}, // =
        {0x00, 0x41, 0x22, 0x14, 0x08, 0x00}, // >
        {0x02, 0x01, 0x51, 0x09, 0x06, 0x00}, // ?
        {0x32, 0x49, 0x79, 0x41, 0x3E, 0x00}, // @
        {0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00}, // A
        {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00}, // B
        {0x3E, 0x41, 0x41, 0x41, 0x22, 0x00}, // C
        {0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00}, // D
        {0x7F, 0x49, 0x49, 0x49, 0x41, 0x00}, // E
        {0x7F, 0x09, 0x09, 0x09, 0x01, 0x00}, // F
        {0x3E, 0x41, 0x41, 0x49, 0x7A, 0x00}, // G
        {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00}, // H
        {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00}, // I
        {0x20, 0x40, 0x41, 0x3F, 0x01, 0x00}, // J
        {0x7F, 0x08, 0x14, 0x22, 0x41, 0x00}, // K
        {0x7F, 0x40, 0x40, 0x40, 0x40, 0x00}, // L
        {0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00}, // M
        {0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00}, // N
        {0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00}, // O
        {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00}, // P
        {0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00}, // Q
        {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00}, // R
        {0x26, 0x49, 0x49, 0x49, 0x32, 0x00}, // S
        {0x01, 0x01, 0x7F, 0x01, 0x01, 0x00}, // T
        {0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00}, // U
        {0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00}, // V
        {0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00}, // W
        {0x63, 0x14, 0x08, 0x14, 0x63, 0x00}, // X
        {0x07, 0x08, 0x70, 0x08, 0x07, 0x00}, // Y
        {0x61, 0x51, 0x49, 0x45, 0x43, 0x00}, // Z
        {0x00, 0x7F, 0x41, 0x41, 0x00, 0x00}, // [
        {0x02, 0x04, 0x08, 0x10, 0x20, 0x00}, // backslash
        {0x00, 0x41, 0x41, 0x7F, 0x00, 0x00}, // ]
        {0x04, 0x02, 0x01, 0x02, 0x04, 0x00}, // ^
        {0x40, 0x40, 0x40, 0x40, 0x40, 0x00}, // _
        {0x00, 0x01, 0x02, 0x04, 0x00, 0x00}, // `
        {0x20, 0x54, 0x54, 0x54, 0x78, 0x00}, // a
        {0x7F, 0x48, 0x44, 0x44, 0x38, 0x00}, // b
        {0x38, 0x44, 0x44, 0x44, 0x20, 0x00}, // c
        {0x38, 0x44, 0x44, 0x48, 0x7F, 0x00}, // d
        {0x38, 0x54, 0x54, 0x54, 0x18, 0x00}, // e
        {0x08, 0x7E, 0x09, 0x01, 0x02, 0x00}, // f
        {0x08, 0x54, 0x54, 0x54, 0x3C, 0x00}, // g
        {0x7F, 0x08, 0x04, 0x04, 0x78, 0x00}, // h
        {0x00, 0x48, 0x7D, 0x40, 0x00, 0x00}, // i
        {0x20, 0x40, 0x44, 0x3D, 0x00, 0x00}, // j
        {0x7F, 0x10, 0x28, 0x44, 0x00, 0x00}, // k
        {0x00, 0x41, 0x7F, 0x40, 0x00, 0x00}, // l
        {0x7C, 0x04, 0x78, 0x04, 0x78, 0x00}, // m
        {0x7C, 0x08, 0x04, 0x04, 0x78, 0x00}, // n
        {0x38, 0x44, 0x44, 0x44, 0x38, 0x00}, // o
        {0x7C, 0x14, 0x14, 0x14, 0x08, 0x00}, // p
        {0x08, 0x14, 0x14, 0x18, 0x7C, 0x00}, // q
        {0x7C, 0x08, 0x04, 0x04, 0x08, 0x00}, // r
        {0x48, 0x54, 0x54, 0x54, 0x20, 0x00}, // s
        {0x04, 0x3F, 0x44, 0x40, 0x20, 0x00}, // t
        {0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00}, // u
        {0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00}, // v
        {0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00}, // w
        {0x44, 0x28, 0x10, 0x28, 0x44, 0x00}, // x
        {0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00}, // y
        {0x44, 0x64, 0x54, 0x4C, 0x44, 0x00}, // z
        {0x00, 0x08, 0x36, 0x41, 0x00, 0x00}, // {
        {0x00, 0x00, 0x7F, 0x00, 0x00, 0x00}, // |
        {0x00, 0x41, 0x36, 0x08, 0x00, 0x00}, // }
        {0x10, 0x08, 0x08, 0x10, 0x08, 0x00}  // ~
};

struct Color
{
  uint8_t r, g, b;

  // Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
  Color(uint16_t rgb565) : r((rgb565 >> 8) & 0xF8), g((rgb565 >> 3) & 0xFC), b((rgb565 << 3) & 0xF8) {}
  Color(uint8_t rgb332) : r((rgb332 & 0xE0) | (rgb332 << 3) & 0x18), g((rgb332 << 5) & 0xE0), b((rgb332 << 6) & 0xC0) {}

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
template <typename BufferFormat>
class DisplayBuffer
{
public:
  DisplayBuffer() : area(0, 0), buffer(nullptr) {}

  void init(Window area)
  {
    if (buffer != nullptr)
      delete[] buffer;
    this->area = area;

    // Adjust size to fit within available memory
    int bufferSize = size();
    buffer = new BufferFormat[bufferSize];

    if (buffer == nullptr)
    {
      Serial.println("[DisplayBuffer] Buffer allocation failed.");
    }
    else
    {
      Serial.print("[DisplayBuffer] ");
      Serial.print(bufferSize * sizeof(BufferFormat));
      Serial.println(" bytes allocated.");
    }

#ifdef DEBUG
    Serial.print("[DisplayBuffer] ");
    Serial.print(bufferSize);
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
    if (x >= 0 && x < area.width && y >= 0 && y < area.height)
    {
      buffer[y * area.width + x] = static_cast<BufferFormat>(color);
    }
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

  void fill(Color color)
  {
    for (int x = 0; x < area.width; x++)
      for (int y = 0; y < area.height; y++)
        set(x, y, color);
  }

  int size() { return min(area.size(), MAX_MEMORY_B / sizeof(BufferFormat)); }
  bool isTruncated() { return area.size() > MAX_MEMORY_B / sizeof(BufferFormat); }

private:
  Window area;
  BufferFormat *buffer;
};

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
                                   buffer(DisplayBuffer<COLOR_FORMAT>()) {}

  void init()
  {
    pinMode(RST, OUTPUT);
    pinMode(DC, OUTPUT);
    pinMode(CS, OUTPUT);

#if defined(__AVR_ATmega2560__)
    pinMode(SS_SLAVE, OUTPUT);
#endif
    digitalWrite(RST, HIGH);
    digitalWrite(DC, HIGH);
    digitalWrite(CS, HIGH);

    SPI.begin();
    delay(10);

#ifdef DEBUG
    Serial.print("[SPI] Initialized with ");
    Serial.println("Hz.");

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
    SPI.transfer(0xA2);
    SPI.transfer(0x02);
    SPI.transfer(0x84);
    writeCommand(PWCRT4);
    SPI.transfer(0x8A);
    SPI.transfer(0x2A);
    writeCommand(PWCRT5);
    SPI.transfer(0x8A);
    SPI.transfer(0xEE);
    writeCommand(VMCTR1);
    SPI.transfer(0x0E); // VCOM = -0.775V
                        // Memory Data Access Control D7/D6/D5/D4/D3/D2/D1/D0
                        //  MY/MX/MV/ML/RGB/MH/-/-
                        //  MY- Row Address Order; ‘0’ =Increment, (Top to Bottom)
                        //  MX- Column Address Order; ‘0’ =Increment, (Left to Right)
                        //  MV- Row/Column Exchange; '0’ = Normal,
                        //  ML- Scan Address Order; ‘0’ =Decrement,(LCD refresh Top to Bottom)
                        //  RGB - '0'= RGB color fill order
                        //  MH - '0'= LCD horizontal refresh left to right

    writeCommand(MADCTL);
    SPI.transfer(0xC8);
    writeCommand(COLMOD);
    SPI.transfer(0x05); // 16-bit/pixel; high nibble don't care

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
  };

  void configureArea(Window area)
  {
    beginTransaction();
    buffer.init(area);
    writeCommand(CASET);
    delay(1000);
    SPI.transfer(area.xs() >> 8);
    SPI.transfer(area.xs() & 0xFF);
    SPI.transfer((area.xe() - 1) >> 8);
    SPI.transfer((area.xe() - 1) & 0xFF);

    writeCommand(RASET);
    SPI.transfer(area.ys() >> 8);
    SPI.transfer(area.ys() & 0xFF);
    SPI.transfer((area.ye() - 1) >> 8);
    SPI.transfer((area.ye() - 1) & 0xFF);

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

    int bufferSize = buffer.size();
    for (int i = 0; i < bufferSize; i++)
    {
      SPI.transfer(((uint16_t)(Color)colors[i]) >> 8);
      SPI.transfer(((uint16_t)(Color)colors[i]) & 0xFF);
    }

    endTransaction();

#ifdef DEBUG
    Serial.print("[Display] Drawn ");
    Serial.print(bufferSize);
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
  void clear()
  {
    configureArea(window);
    fill(Color::black);
    drawPixels();

#ifdef DEBUG
    Serial.println("[Display] Cleared.");
#endif
  };

  int printChar(int x, int y, char value, Color fgColor, Color bgColor)
  {

    if (x + CHAR_WIDTH > window.width || y + CHAR_HEIGHT > window.height)
      return -1;

    for (int i = 0; i < CHAR_WIDTH; i++)
      for (int j = 0; j < CHAR_HEIGHT; j++)
        if ((font[value - ' '][i]) & (1 << (CHAR_HEIGHT - 1 - j)))
          setPixel(x + i, y + j, fgColor);
        else
          setPixel(x + i, y + j, bgColor);

    drawPixels();

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
    if (x + strlen(c_str) * 6 > window.width || y + 8 > window.height)
      return -1;
    else
      for (int i = 0; c_str[i] != '\0'; i++)
        printChar(x + i * 6, y, c_str[i], fgColor, bgColor);

    return 0;
  }

private:
  const static int CHAR_WIDTH = 6, CHAR_HEIGHT = 8;

  const static int RST = 9, DC = 8;

#ifdef __AVR_ATmega2560__
  const static int CS = 53;
  typedef uint8_t COLOR_FORMAT;
#else
  const static int CS = 10;
  typedef uint16_t COLOR_FORMAT;
#endif

#define SPI_DEFAULT_FREQ 1e6
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

  void writeData(int count, ...)
  {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; ++i)
      SPI.transfer(va_arg(args, uint8_t));

    va_end(args);
  }

public:
  const static Display ili9341, st7735, limitedMemory;
};

const Display
    Display::ili9341 = Display(240, 320),     // 2.2" TFT, 240x320 pixel
    Display::st7735 = Display(64, 64),        // 1.8" TFT, 160x128 pixel
    Display::limitedMemory = Display(64, 64); // 64x64 pixel

#ifdef __AVR_ATmega2560__
Display display = Display::st7735;
#else
Display display = Display::st7735;
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

void setup1()
{
  Serial.begin(9600); // open serial port
  Serial.println("start");
  display.init(); // power-on-reset of Display
}

void loop1()
{
  columns();
}

void activity(int length, int delayMS)
{
  const int xc = display.window.width / 2;
  const int yc = display.window.height / 2;
  const int radius = length / 2;

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

void activityIndicator()
{
  noInterrupts();
  Serial.println("activityIndicator");
  activity(20, 10000 / 5);
  interrupts();
}
void setup2()
{
  // // Enable the external memory interface
  // MCUCR |= (1 << SRE);

  // // Double-check the SRE bit is set
  // if (!(MCUCR & (1 << SRE))) {
  //   Serial.println("[Error] Failed to enable external memory interface.");
  //   while (1); // Halt execution if enabling fails
  // }

  Serial.begin(9600); // open serial port
  display.init();     // power-on-reset of Display

  Serial.println("[Exercise 6.4] Setup done.");

  Timer::configure(activityIndicator);
  Timer::start();
}

void loop2() {}

void printChar(char c)
{
  int result = display.printChar(10, 10, c, Color::white, Color::black);

  Serial.print("[Exercise 6.5] Character ");
  Serial.print(c);
  Serial.println((result == -1) ? " not printed." : " printed.");
}

void printString(char *str)
{
  int result = display.printString(30, 20, str, Color::green, Color::black);

  Serial.print("[Exercise 6.5] String ");
  Serial.print(str);
  Serial.println((result == -1) ? " not printed." : " printed.");
}

void setup3()
{
  Serial.begin(9600); // open serial port
  display.init();     // power-on-reset of Display
  Serial.println("[Exercise 6.5] Setup done.");

  display.clear();

  printChar('A');
  printString("Hi!");
}

void loop3() {}

char *names[] = {"Fritz", "Leopold"},
     *matriculationNumbers[] = {"0000000", "7724741"};

static int student = 0;

void printNameAndID()
{
  const int xc = display.window.width / 2, yc = display.window.height / 2;

  display.clear();
  display.printString(xc - strlen(names[student]) * 6 / 2, yc + 2 + 8, names[student], Color::white, Color::black);
  display.printString(xc - strlen(matriculationNumbers[student]) * 6 / 2, yc - 3, matriculationNumbers[student], Color::white, Color::black);
}

void printNextStudent()
{
  printNameAndID();

  Serial.print("[Exercise 6.6] Printed student ");
  Serial.print(student);
  Serial.print(" (");
  Serial.print(names[student]);
  Serial.print(", ");
  Serial.print(matriculationNumbers[student]);
  Serial.println(").");

  student = (student + 1) % 2;
}

void setup()
{
  Serial.begin(9600); // open serial port
  display.init();     // power-on-reset of Display

  Serial.println("[Exercise 6.6] Setup done.");
  Serial.println("[Exercise 6.6] Starting student id demo.");

  Timer::configure(printNextStudent, 5000);
  Timer::start();
}

void loop() {}
