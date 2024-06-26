#include "Display.h"
#include "Timer.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

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