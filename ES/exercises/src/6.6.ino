#include "Display.h"
#include "Timer.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

char *names[] = {"Fritz Foerster", "Leopold Lemmermann"},
     *matriculationNumbers[] = {"0000000", "7724741"};

static int student = 0;

void printNextStudent()
{
  const int xc = display.getDimensions().width / 2, yc = display.getDimensions().height / 2;

  display.clear();
  display.printString(
      xc - strlen(names[student]) * 6 / 2,
      yc + 2 + 8,
      names[student], Color::white, Color::black);
  display.printString(
      xc - strlen(matriculationNumbers[student]) * 6 / 2,
      yc - 3,
      matriculationNumbers[student], Color::white, Color::black);

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
  Serial.begin(9600);
  display.init();

  Serial.println("[Exercise 6.6] Setup done.");
  Serial.println("[Exercise 6.6] Starting student id demo.");

  Timer::configure(printNextStudent, 5000);
  Timer::start();
}

void loop() {}