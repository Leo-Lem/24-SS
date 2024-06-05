// ES-exercise 06: Demo to initialize TFT-Display with ST7735R controller, e.g. joy-it RB-TFT1.8-V2. configuration:  4-line serial interface, RGB-order: R-G-B

#include <SPI.h>

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

#if defined(__AVR_ATmega2560__)
#define SS_SLAVE 53
#endif

#define TFT_DC_HIGH() digitalWrite(TFT_DC, HIGH)
#define TFT_DC_LOW() digitalWrite(TFT_DC, LOW)
#define TFT_CS_HIGH() digitalWrite(TFT_CS, HIGH)
#define TFT_CS_LOW() digitalWrite(TFT_CS, LOW)

// SPI-Settings
#define SPI_DEFAULT_FREQ 1e6 ///< Default SPI data clock frequency
SPISettings settingsTFT(SPI_DEFAULT_FREQ, MSBFIRST, SPI_MODE0);

// TFT-area of 128 x 160 (1.8") TFT
#define FIRST_COL 2
#define FIRST_ROW 1
#define LAST_COL 129
#define LAST_ROW 160

// TFT's commands
#define NOP 0x00     // no Operation
#define SWRESET 0x01 // Software reset
#define SLPOUT 0x11  // Sleep out & booster on
#define DISPOFF 0x28 // Display off
#define DISPON 0x29  // Display on
#define CASET 0x2A   // Column adress set
#define RASET 0x2B   // Row adress set
#define RAMWR 0x2C   // Memory write
#define MADCTL 0x36  // Memory Data Access Control
#define COLMOD 0x3A  // RGB-format 12/16/18bit
#define INVOFF 0x20  // Display inversion off
#define INVON 0x21   // Display inversion on
#define INVCTR 0xB4  // Display Inversion mode control
#define NORON 0x13   // Partial off (Normal)

#define PWCTR1 0xC0 // Power Control 1
#define PWCTR2 0xC1 // Power Control 2
#define PWCTR3 0xC2 // Power Control 3
#define PWCTR4 0xC3 // Power Control 4
#define PWCTR5 0xC4 // Power Control 5
#define VMCTR1 0xC5 // VCOM Voltage setting

// global variables
int invState = 0;

void TFTwriteCommand(int cmd)
{
  TFT_DC_LOW();
  SPI.transfer(cmd);
  TFT_DC_HIGH();
}

void TFTwrite_saCommand(int cmd)
{
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
  TFT_DC_LOW();
  SPI.transfer(cmd);
  TFT_DC_HIGH();
  TFT_CS_HIGH();
  SPI.endTransaction();
}

void TFTwriteWindow(int xs, int xe, int ys, int ye)
{
  // test whether parameters stay within address ranges;  should be implemented//
  TFTwriteCommand(NOP);
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
  //  MY- Row Address Order; ‘0’ =Increment, (Top to Bottom)
  //  MX- Column Address Order; ‘0’ =Increment, (Left to Right)
  //  MV- Row/Column Exchange; '0’ = Normal,
  //  ML- Scan Address Order; ‘0’ =Decrement,(LCD refresh Top to Bottom)
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
  uint8_t xs = FIRST_COL;
  uint8_t xe = LAST_COL;
  uint8_t ys = FIRST_ROW;
  uint8_t ye = LAST_ROW;
  uint16_t time = millis();
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
  TFTwriteWindow(xs, xe, ys, ye);
  TFTwriteCommand(RAMWR); // assign background-color to every element of writewindow
  for (uint16_t i = 0; i < (xe + 1 - xs) * (ye + 1 - ys); i++)
  {
    SPI.transfer(0xFF);
    SPI.transfer(0xFF);
  }
  TFT_CS_HIGH();
  SPI.endTransaction();
  time = millis() - time;
  Serial.print("time consumption of clear-display: ");
  Serial.print(time, DEC);
  Serial.println(" ms");

  // draw blue rectangle (50x50pixel) centered on Display
  uint8_t xc = FIRST_COL + ((LAST_COL + 1 - FIRST_COL) >> 1);
  uint8_t yc = FIRST_ROW + ((LAST_ROW + 1 - FIRST_ROW) >> 1);
  xs = xc - 25;
  xe = xc + 25;
  ys = yc - 25;
  ye = yc + 25;

  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
  TFTwriteWindow(xs, xe, ys, ye);
  TFTwriteCommand(RAMWR); // assign blue-color to every element of writewindow
  for (uint16_t i = 0; i < (xe + 1 - xs) * (ye + 1 - ys); i++)
  {
    SPI.transfer((uint8_t)(0x001F >> 8));
    SPI.transfer((uint8_t)0x001F);
  }
  TFT_CS_HIGH();
  SPI.endTransaction();

  delay(500);

  // draw centered red colored cross with one pixel space to border
  xs = FIRST_COL + 1;
  xe = LAST_COL - 1;
  ys = FIRST_ROW + 1;
  ye = LAST_ROW - 1;
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
  // horizontal line
  TFTwriteWindow(xs, xe, yc, yc);
  TFTwriteCommand(RAMWR); // assign background-color to every element of writewindow
  for (uint16_t i = 0; i < (xe + 1 - xs) * (ye + 1 - ys); i++)
  {
    SPI.transfer((uint8_t)(0xF800 >> 8));
    SPI.transfer((uint8_t)0xF800);
  }
  // vertical line
  TFTwriteWindow(xc, xc, ys, ye);
  TFTwriteCommand(RAMWR); // assign background-color to every element of writewindow
  for (uint16_t i = 0; i < (xe + 1 - xs) * (ye + 1 - ys); i++)
  {
    SPI.transfer((uint8_t)(0xF800 >> 8));
    SPI.transfer((uint8_t)0xF800);
  }
  TFT_CS_HIGH();
  SPI.endTransaction();
  Serial.println("\nSetup finished\n");
}

void loop()
{
  delay(1000);
  (invState) ? TFTwrite_saCommand(INVON) : TFTwrite_saCommand(INVOFF);
  invState = !invState;
}
