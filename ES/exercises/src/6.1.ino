#include "Display.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

void demo()
{
// clear display
#ifdef __AVR_ATmega2560__
  Window area = Window(80, 80);
#else
  Window area = display.window;
#endif

  display.configureArea(area);
  display.fill(Color::white);
  display.drawPixels();

  // draw blue rectangle centered on Display
  const int diameter = 10;
  const int xc = area.width / 2, yc = area.height / 2;

  display.configureArea(Window(xc - diameter, xc + diameter, yc - diameter, yc + diameter));
  display.fill(Color::blue);
  display.drawPixels();

  // draw centered red colored cross with one pixel space to border
  display.configureArea(Window(area.xs(), area.xe(), yc, yc + 1));
  for (int x = 0; x < area.width; x++)
    display.setPixel(x, 0, Color::red);

  display.drawPixels();

  display.configureArea(Window(xc, xc + 1, area.ys(), area.ye()));
  for (int y = 0; y < area.height; y++)
    display.setPixel(0, y, Color::red);

  display.drawPixels();
};

// void setup()
// {
//   Serial.begin(9600); // open serial port
//   Serial.println("[Demo] TFT-LCD demo starting…");

//   display.init(); // power-on-reset of Display

//   demo(); // draw the demo image
//   Serial.println("[Demo] Setup done.");
// }


void setup() {
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

  //power-on-reset of Display
  digitalWrite(TFT_RST, HIGH);
  delay(100);
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(100);

  TFTinit();
  Serial.println("Display Initialized");
  delay(100);

  //clear display
    uint8_t xs = FIRST_COL;
    uint8_t xe = LAST_COL;
    uint8_t ys = FIRST_ROW;
    uint8_t ye = LAST_ROW;
  uint16_t time = millis();
    SPI.beginTransaction(settingsTFT);
    TFT_CS_LOW();
    TFTwriteWindow(xs, xe, ys, ye);
    TFTwriteCommand(RAMWR);  //assign background-color to every element of writewindow
        for (uint16_t i=0; i<(xe+1-xs)*(ye+1-ys); i++) {
            SPI.transfer(0xFF);
            SPI.transfer(0xFF);}
    TFT_CS_HIGH();
    SPI.endTransaction();
  time = millis() - time;
  Serial.print("time consumption of clear-display: "); Serial.print(time, DEC); Serial.println(" ms");

    //draw blue rectangle (50x50pixel) centered on Display
    uint8_t  xc = FIRST_COL + ((LAST_COL+1 - FIRST_COL) >>1);
    uint8_t  yc = FIRST_ROW + ((LAST_ROW+1 - FIRST_ROW) >>1);
    xs = xc - 25;
    xe = xc + 25;
    ys = yc - 25;
    ye = yc + 25;

    SPI.beginTransaction(settingsTFT);
    TFT_CS_LOW();
    TFTwriteWindow(xs, xe, ys, ye);
    TFTwriteCommand(RAMWR);  //assign blue-color to every element of writewindow
        for (uint16_t i=0; i<(xe+1-xs)*(ye+1-ys); i++) {
            SPI.transfer((uint8_t)(0x001F>>8));
            SPI.transfer((uint8_t)0x001F); }
    TFT_CS_HIGH();
    SPI.endTransaction();

    delay(500);

    //draw centered red colored cross with one pixel space to border
    xs = FIRST_COL+1;
    xe = LAST_COL-1;
    ys = FIRST_ROW+1;
    ye = LAST_ROW-1;
    SPI.beginTransaction(settingsTFT);
    TFT_CS_LOW();
    //horizontal line
    TFTwriteWindow(xs, xe, yc, yc);
    TFTwriteCommand(RAMWR);  //assign background-color to every element of writewindow
        for (uint16_t i=0; i<(xe+1-xs)*(ye+1-ys); i++) {
            SPI.transfer((uint8_t)(0xF800>>8));
            SPI.transfer((uint8_t)0xF800); }
    //vertical line
    TFTwriteWindow(xc, xc, ys, ye);
    TFTwriteCommand(RAMWR);  //assign background-color to every element of writewindow
        for (uint16_t i=0; i<(xe+1-xs)*(ye+1-ys); i++) {
            SPI.transfer((uint8_t)(0xF800>>8));
            SPI.transfer((uint8_t)0xF800); }
    TFT_CS_HIGH();
    SPI.endTransaction();
  Serial.println("\nSetup finished\n");
}

bool isInverted = false;

void loop()
{
  delay(1000);
  display.invert(isInverted = !isInverted);
  Serial.print("[Demo] Color inversion is ");
  Serial.println(isInverted ? "on." : "off.");
}
