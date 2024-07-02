// ES-exercise 06                                                                   //
// Demo to initialize TFT-Display with ST7735R controller,          //
// e.g. joy-it RB-TFT1.8-V2.                                         //
// configuration:  4-line serial interface, RGB-order: R-G-B,        //

#include <SPI.h>
#include <SD.h>

// pin declarations
#define TFT_CS 10 // display: CS-pin
#define TFT_RST 9 // display: reset
#define TFT_DC 8  // display: Data/Command (D/C)
#define SD_CS 4   // SD-card: CS-pin

#if defined(__AVR_ATmega2560__)
#define SS_SLAVE 53
// must be put into output mode; otherwise ATmega could assume
// to be set into slave mode but SPI-lib doesn't support this
// mode. So it breaks SPI-lib. Configured as output, the pin
// can be used as normal output.
#endif

#define TFT_DC_HIGH() digitalWrite(TFT_DC, HIGH)
#define TFT_DC_LOW() digitalWrite(TFT_DC, LOW)
#define TFT_CS_HIGH() digitalWrite(TFT_CS, HIGH)
#define TFT_CS_LOW() digitalWrite(TFT_CS, LOW)

// SPI-Settings
#define SPI_DEFAULT_FREQ 1e6 ///< Default SPI data clock frequency
SPISettings settingsTFT(SPI_DEFAULT_FREQ, MSBFIRST, SPI_MODE0);

// TFT-area of 128 x 160 (1.8") TFT
const uint8_t FIRST_COL = 2;
const uint8_t FIRST_ROW = 1;
const uint8_t LAST_COL = 129;
const uint8_t LAST_ROW = 160;

const uint8_t CENTER_X = (LAST_COL + 1) / 2;
const uint8_t CENTER_Y = (LAST_ROW + 1) / 2;

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

uint8_t cur_col = FIRST_COL;
const uint8_t buffercol_size = 22;
const uint16_t maxBit = 5168;
uint8_t framebuffer[maxBit];
uint16_t pointer = 0;
const uint8_t bgColor = 0;
uint8_t curColor;
const uint8_t fgColor = 255;
uint8_t posi = 0;

// global variables
uint8_t invState = 0;

void TFTwriteCommand(uint8_t cmd)
{
  TFT_DC_LOW();
  SPI.transfer(cmd);
  TFT_DC_HIGH();
}

void TFTwrite_saCommand(uint8_t cmd)
{
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
  TFT_DC_LOW();
  SPI.transfer(cmd);
  TFT_DC_HIGH();
  TFT_CS_HIGH();
  SPI.endTransaction();
}
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

void TFTwriteWindow(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye)
{
  // test weather parameters stay within address ranges;  should be implemented//
  TFTwriteCommand(NOP); // normally not neccessary; but if not, the first command after eg. SD-access will be ignored (here: CASET)
  TFTwriteCommand(CASET);
  SPI.transfer(0x00);
  SPI.transfer(xs);
  SPI.transfer(0x00);
  SPI.transfer(xe);
  TFTwriteCommand(RASET);
  SPI.transfer(0x00);
  SPI.transfer(ys);
  SPI.transfer(0x00);
  SPI.transfer(ye);
}

void TFTinit(void)
{
  // minimal configuration: only settings which are different from Reset Default Value
  // or not affected by HW/SW-reset
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();

  TFTwriteCommand(SWRESET);
  delay(120);              // mandatory delay
  TFTwriteCommand(SLPOUT); // turn off sleep mode.
  delay(120);
  TFTwriteCommand(PWCTR1);
  SPI.transfer(0xA2);
  SPI.transfer(0x02);
  SPI.transfer(0x84);
  TFTwriteCommand(PWCTR4);
  SPI.transfer(0x8A);
  SPI.transfer(0x2A);
  TFTwriteCommand(PWCTR5);
  SPI.transfer(0x8A);
  SPI.transfer(0xEE);
  TFTwriteCommand(VMCTR1);
  SPI.transfer(0x0E); // VCOM = -0.775V

  // Memory Data Access Control D7/D6/D5/D4/D3/D2/D1/D0
  //                                                        MY/MX/MV/ML/RGB/MH/-/-
  //  MY- Row Address Order; â€˜0â€™ =Increment, (Top to Bottom)
  //  MX- Column Address Order; â€˜0â€™ =Increment, (Left to Right)
  //  MV- Row/Column Exchange; '0â€™ = Normal,
  //  ML- Scan Address Order; â€˜0â€™ =Decrement,(LCD refresh Top to Bottom)
  // RGB - '0'= RGB color fill order
  //  MH - '0'= LCD horizontal refresh left to right
  TFTwriteCommand(MADCTL);
  SPI.transfer(0x08);

  // RGB-format
  TFTwriteCommand(COLMOD); // color mode
  SPI.transfer(0x55);      // 16-bit/pixel; high nibble don't care

  TFTwriteCommand(CASET);
  SPI.transfer(0x00);
  SPI.transfer(FIRST_COL);
  SPI.transfer(0x00);
  SPI.transfer(LAST_COL);
  TFTwriteCommand(RASET);
  SPI.transfer(0x00);
  SPI.transfer(FIRST_ROW);
  SPI.transfer(0x00);
  SPI.transfer(LAST_ROW);

  TFTwriteCommand(NORON);
  TFTwriteCommand(DISPON);

  TFT_CS_HIGH();
  SPI.endTransaction();
}

uint16_t calculate_uint8color(uint8_t value)
{

  uint16_t color565 = 0;
  uint8_t green_mask = 28;
  color565 = color565 + ((value >> 5) << 13);
  color565 = color565 + ((uint16_t)(value & green_mask) << 6);
  color565 = color565 + ((uint8_t)(value << 6) >> 3);
  return color565;
}

void transmit_framebuffer(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye)
{
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
  TFTwriteWindow(xs, xe, ys, ye);
  TFTwriteCommand(RAMWR);
  uint16_t color;
  for (uint16_t i = 0; i < pointer; i++)
  {
    color = calculate_uint8color(framebuffer[i]);
    SPI.transfer((uint8_t)(color >> 8));
    SPI.transfer((uint8_t)(color));
  }
  TFT_CS_HIGH();
  SPI.endTransaction();
  pointer = 0;
}
void write_Col()
{
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
  for (int i = 0; i < LAST_ROW; i++)
  {
    framebuffer[i] = curColor;
  }
  pointer = LAST_ROW;
  uint8_t xs = cur_col;
  uint8_t xe = cur_col;
  uint8_t ys = FIRST_ROW + 1;
  uint8_t ye = LAST_ROW - 1;
  pointer = LAST_ROW;
  transmit_framebuffer(xs, xe, ys, ye);
}
void setPixel(uint8_t x, uint8_t y, uint8_t value)
{
  framebuffer[0] = value;
  pointer = 1;
  transmit_framebuffer(x, x, y, y);
}
void clearDisplay()
{
  uint8_t xs = FIRST_COL;
  uint8_t xe = LAST_COL;
  uint8_t ys = FIRST_ROW;
  uint8_t ye = LAST_ROW;
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
  TFTwriteWindow(xs, xe, ys, ye);
  TFTwriteCommand(RAMWR); // assign background-color to every element of writewindow
  for (uint16_t i = 0; i < (uint16_t)((xe + 1 - xs) * (ye + 1 - ys)); i++)
  {
    SPI.transfer(bgColor);
    SPI.transfer(bgColor);
  }
  TFT_CS_HIGH();
  SPI.endTransaction();
}
void setup()
{
  // set pin-modes
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_CS, OUTPUT);
#if defined(__AVR_ATmega2560__)
  pinMode(SS_SLAVE, OUTPUT);
#endif

  // set inactive levels
  digitalWrite(TFT_RST, HIGH);
  digitalWrite(TFT_DC, HIGH);
  digitalWrite(TFT_CS, HIGH);

  // initialize serial port 0
  Serial.begin(9600);
  Serial.println("Exercise init TFT template\n");

  // initialize SPI:
  // several devices: multiple SPI.begin(nn_CS) possible
  SPI.begin();
  delay(10);

  // power-on-reset of Display
  digitalWrite(TFT_RST, HIGH);
  delay(100);
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(100);

  TFTinit();
  Serial.println("Display Initialized");
  delay(100);

  // clear display
  uint16_t time = millis();
  clearDisplay();
  time = millis() - time;
  Serial.print("time consumption of clear-display: ");
  Serial.print(time, DEC);
  Serial.println(" ms");

  curColor = fgColor;

  if (!SD.begin(SD_CS))
    Serial.println("Failed to initialise SD connection.");

  Serial.println("\nSetup finished\n");
}

int charToIndex(char c)
{
  if (c >= ' ' && c <= '~')
  { // Check if the character is in the printable ASCII range
    return c - ' ';
  }
  return -1; // Return -1 if the character is not in the printable ASCII range
}

void printChar(uint8_t x, uint8_t y, unsigned char value[6], uint16_t fg_Color, uint16_t bg_Color)
{
  uint8_t xs = x;
  uint8_t xe = x + 7;
  uint8_t ys = y;
  uint8_t ye = y + 5;
  if (xe > LAST_COL || ye > LAST_ROW)
  {
    Serial.println("Out of Bounds");
    return;
  }
  pointer = 8 * 6;
  for (int j = 0; j < 6; j++)
  {
    uint8_t cur_row = value[j];
    for (int i = 0; i < 8; i++)
    {
      if (cur_row & (1 << (7 - j)))
      {
        framebuffer[i * 8 + j] = fg_Color;
      }
      else
      {
        framebuffer[i * 8 + j] = bg_Color;
      }
    }
  }
  transmit_framebuffer(xs, xe, ys, ye);
}
void printString(uint8_t x, uint8_t y, char *c_str, uint16_t fg_Color, uint16_t bg_Color)
{
  uint8_t x_start = x, y_start = y, y_max = 0, cur_row = 0, max_row = 0;

  for (int i = 0; i < strlen(c_str); i++)
    printChar(x_start, y_start + i * 6, font[charToIndex(c_str[i])], fg_Color, bg_Color);
}

bool demoIsRunning = false;

void runStudentIdDemo()
{
  char ff[] PROGMEM = "Fritz Forster";
  char mff[] PROGMEM = "012345678";

  demoIsRunning = true;

  while (demoIsRunning)
  {
    printString(110, 10, ff, fgColor, bgColor);
    printString(102, 10, mff, fgColor, bgColor);
    delay(5000);
    clearDisplay();
    printString(110, 10, "Leopold Lemmermann", fgColor, bgColor);
    printString(102, 10, "987654321", fgColor, bgColor);
    delay(5000);
    clearDisplay();
  }
}
void listDirectory(String dirName)
{
  File dir = SD.open(dirName);
  if (!dir)
    Serial.println("Directory does not exist.");
  else
    while (File file = dir.openNextFile())
    {
      Serial.print("  ");
      Serial.println(file.name());
      file.close();
    }

  dir.close();
}
void doesFileExist(String fileName)
{
  Serial.println(SD.exists(fileName) ? "File exists." : "File does not exist.");
}
void outputFileToSerial(String fileName)
{
  File file = SD.open(fileName);
  if (!file)
  {
    Serial.println("File does not exist.");
    return;
  }

  while (file.available())
    Serial.write(file.read());

  file.close();
}
void outputFileToLCD(String fileName)
{
  File file = SD.open(fileName);
  if (!file)
  {
    Serial.println("File does not exist.");
    return;
  }

  clearDisplay();

  if (fileName.endsWith(".txt"))
  {
    const int baseX = 10,  baseY = LAST_COL - 50;
    int x = baseX, y = baseY;

    int lineWidth = LAST_COL + 1 - FIRST_COL - baseX;

    while (file.available())
    {
      char c = file.read();
      if (c == '\n')
      {
        x = baseX, y--;
        continue;
      }
      else if ((x-baseX) > (lineWidth / 6))
        x = baseX, y--;
      else
        x++;

      printChar((x-baseX) * 6 + baseX,(y-baseY) * 8 + baseY , font[charToIndex(c)], fgColor, bgColor);
    }
  }

  // .img files
  // format: first line contains dimensions, such as 48,48\n
  // format: the following lines contain the pixel
  else if (fileName.endsWith(".img"))
  {
    int dimensionX, dimensionY;

    byte d1 = file.read();
    byte d2 = file.read();
    dimensionX = (d1 - '0') * 10 + (d2 - '0');
    file.read();
    d1 = file.read();
    d2 = file.read();
    dimensionY = (d1 - '0') * 10 + (d2 - '0');
    file.read();

    const int baseX = CENTER_X - dimensionX, baseY = CENTER_Y - dimensionY;
    int x = baseX, y = baseY;
    while (file.available())
    {
      byte b = file.read();
      setPixel(x, y, (b == '1') ? fgColor : bgColor);
      if (++x >= 2 * dimensionX + baseX)
        x = baseX, y += 2;
    }
  }

  file.close();
}

void help();
void runRotatingBarDemo();
void stopDemo();

// Main loop function
void loop()
{
  if (Serial.available() > 0)
  {
    String input = Serial.readStringUntil('\n'); // Read the input from the serial monitor
    input.trim();                                // Remove any leading or trailing whitespace
    Serial.println("Entered command: " + input); // Print the entered command
    parseCommand(input);
  }
}

// Function to parse the command
void parseCommand(String command)
{
  if (command.equals("help"))
  {
    help();
  }
  else if (command.equals("clearDisplay") || command.equals("clear"))
  {
    clearDisplay();
  }
  else if (command.equals("runRotatingBarDemo") || command.equals("runRBD"))
  {
    runRotatingBarDemo();
  }
  else if (command.equals("runStudentIdDemo") || command.equals("runID"))
  {
    runStudentIdDemo();
  }
  else if (command.equals("stopDemo") || command.equals("stop"))
  {
    stopDemo();
  }
  else if (command.startsWith("listDirectory") || command.startsWith("ls"))
  {
    command.remove(0, command.indexOf(' ') + 1);
    listDirectory(command);
  }
  else if (command.startsWith("doesFileExist") || command.startsWith("exist"))
  {
    command.remove(0, command.indexOf(' ') + 1);
    doesFileExist(command);
  }
  else if (command.startsWith("outputFileToSerial") || command.startsWith("catSer"))
  {
    command.remove(0, command.indexOf(' ') + 1);
    outputFileToSerial(command);
  }
  else if (command.startsWith("outputFileToLCD") || command.startsWith("catTFT"))
  {
    command.remove(0, command.indexOf(' ') + 1);
    outputFileToLCD(command);
  }
  else
  {
    Serial.println("Error: Unrecognized command. Type 'help' for a list of valid commands.");
  }
}

// Function definitions
void help()
{
  Serial.println("List of valid commands:");
  Serial.println("  help - Lists the valid commands with information on how to use them.");
  Serial.println("  clearDisplay or clear - Clears the buffer and updates the display.");
  Serial.println("  runRotatingBarDemo or runRBD - Starts the Rotating Bar Demo.");
  Serial.println("  runStudentIdDemo or runID - Starts the Student ID Demo.");
  Serial.println("  stopDemo or stop - Stops the execution of the currently running demo.");
  Serial.println("  listDirectory <dir name> |ls <dir name> - Lists all files in the directory specified by <dir name>.");
  Serial.println("  doesFileExist <file name>|exist <file name> - Checks if the file specified by <file name> exists.");
  Serial.println("  outputFileToSerial <file name>|catSer <file name> - Outputs the raw content of the file specified by <file name> to the serial monitor.");
  Serial.println("  outputFileToLCD <file name>|catTFT <file name> - Outputs the (converted) content of the file specified by <file name> to the display.");
}

void rot_balken2(uint8_t durchm, uint8_t balken_color, uint8_t cur_position, boolean clear_flag)
{

  // uint16_t bit8_fg = calculate_uint8color(fgColor);
  // uint16_t bit8_bg = calculate_uint8color(bgColor);
  uint8_t xc = FIRST_COL + ((LAST_COL + 1 - FIRST_COL) >> 1);
  uint8_t yc = FIRST_ROW + ((LAST_ROW + 1 - FIRST_ROW) >> 1);

  uint8_t xs, xe, ys, ye;
  uint16_t size = (xe + 1 - xs) * (ye + 1 - ys);

  // Clear previous
  if (clear_flag)
  {
    rot_balken2(durchm, bgColor, (cur_position + 3) % 4, false);
    delay(20);
  }
  // Write New
  switch (cur_position)
  {
  case 0: // Vertical bar
    xs = xc;
    xe = xc;
    ys = yc - (durchm / 2);
    ye = yc + (durchm / 2);
    size = (xe + 1 - xs) * (ye + 1 - ys);
    if (size > maxBit)
    {
      Serial.println("Too big");
      return;
    }
    for (uint16_t i = 0; i < size; i++)
    {
      framebuffer[i] = balken_color;
    }
    pointer = size;
    transmit_framebuffer(xs, xe, ys, ye);
    break;

  case 1: // 45-degree diagonal
    xs = xc - (durchm / 2);
    xe = xc + (durchm / 2);
    ys = yc - (durchm / 2);
    ye = yc + (durchm / 2);
    for (uint8_t y = ys; y <= ye; y++)
    {
      for (uint8_t x = xs; x <= xe; x++)
      {
        if ((x - xs) == (y - ys))
        {
          setPixel(x, y, balken_color);
        }
      }
    }
    // pointer = size;
    // Serial.println("transmit");
    // transmit_framebuffer(xs, xe, ys, ye);
    break;

  case 2: // Horizontal bar
    xs = xc - (durchm / 2);
    xe = xc + (durchm / 2);
    ys = yc;
    ye = yc;
    size = (xe + 1 - xs) * (ye + 1 - ys);
    if (size > maxBit)
    {
      Serial.println("Too big");
      return;
    }
    for (uint16_t i = 0; i < size; i++)
    {
      framebuffer[i] = balken_color;
    }
    pointer = size;
    transmit_framebuffer(xs, xe, ys, ye);
    break;

  case 3: // 135-degree diagonal
    xs = xc - (durchm / 2);
    xe = xc + (durchm / 2);
    ys = yc - (durchm / 2);
    ye = yc + (durchm / 2);
    for (uint8_t y = ys; y <= ye; y++)
    {
      for (uint8_t x = xs; x <= xe; x++)
      {
        if ((x - xs) == (ye - y))
        {
          setPixel(x, y, balken_color);
        }
      }
    }
    break;

  default:
    break;
  }
}

void runRotatingBarDemo()
{
  // Implementation of runRotatingBarDemo function
  Serial.println("Rotating Bar Demo started.");

  demoIsRunning = true;

  while (demoIsRunning)
  {
    rot_balken2(20, fgColor, posi, true);
    posi = (posi + 1) % 4;
    delay(50);
  }
}

void stopDemo()
{
  demoIsRunning = false;
  // Implementation of stopDemo function
  Serial.println("Demo stopped.");
}