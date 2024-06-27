
#include "Display.h"
#include "Timer.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

void help()
{
  Serial.println("help() - Print a list of commands.");
  Serial.println("clearDisplay()|clear() - Delete the buffer and clear the display.");
  Serial.println("runCrossDemo()|cross() - Start the cross demo (from the example).");
  Serial.println("runRotatingBarDemo()|rbd() - Start the rotating bar demo (from exercise 6.4).");
  Serial.println("runStudentIdDemo()|id() - Start the student ID demo (from exercise 6.6).");
  Serial.println("stopDemo()|stop() - Stop the running demo.");
}
void clearDisplay()
{
  display.clear();
  Serial.println("[Interactive Display] Cleared display.");
}
void runCrossDemo()
{
  Serial.println("[Interactive Display] Running cross demo…");
  Timer::configure(crossDemo, 2000);
  Timer::start();
}
void runRotatingBarDemo()
{
  Serial.println("[Interactive Display] Running rotating bar demo…");
  Timer::configure(activityDemo, 1000);
  Timer::start();
}
void runStudentIdDemo()
{
  Serial.println("[Interactive Display] Running student ID demo…");
  Timer::configure(studentIdDemo, 5000);
  Timer::start();
}
void stopDemo()
{
  Timer::stop();
  Serial.println("[Interactive Display] Stopped demo.");
}

bool crossDemoIsSetUp = false;
bool isInverted = false;

int activityState = 0;

int student = 0;
const char *names[2] = {"Fritz Foerster", "Leopold Lemmermann"},
           *matriculationNumbers[2] = {"0000000", "7724741"};

void crossDemo()
{
  if (!crossDemoIsSetUp)
  {
    crossDemoIsSetUp = true;
    Window area = display.getDimensions();

    display.clear();
    display.fill(Color::blue, Window(area, 20, 20)); // draw centered blue rectangle

    // draw centered red colored cross with two pixel width
    const int yc = area.ys() + area.height / 2;
    display.configureBuffer(Window(area.xs(), yc - 1, area.xe(), yc + 1));
    for (int x = 0; x < area.width; x++)
      display.setPixel(x, 0, Color::red);

    display.drawBuffer();

    const int xc = area.xs() + area.width / 2;
    display.configureBuffer(Window(xc - 1, area.ys(), xc + 1, area.ye()));
    for (int y = 0; y < area.height; y++)
      display.setPixel(0, y, Color::red);

    display.drawBuffer();

    Serial.println("[Cross Demo] Setup done.");
  }

  display.invert(isInverted = !isInverted);

  Serial.print("[Cross Demo] Inverted colors. (");
  Serial.println(isInverted ? "on)" : "off)");
}
void activityDemo()
{
  int length = 50;

  Window area(display.getDimensions(), length, length);

  const int
      xc = area.xs() + area.width / 2,
      yc = area.ys() + area.height / 2,
      radius = length / 2;

  switch (activityState)
  {
  case 0: // symbol |
    display.clear(area);
    display.fill(Color::white, Window(xc, yc - radius, xc + 1, yc + radius));
    Serial.print("[Activity Demo] Symbol | drawn.\r");
    break;
  case 1: // symbol /
    display.clear(area);
    display.configureBuffer(area);
    for (int i = 0; i < length; i++)
      display.setPixel(area.width - i, i, Color::white);
    display.drawBuffer();

    Serial.print("[Activity Demo] Symbol / drawn.\r");
    break;
  case 2: // symbol -
    display.clear(area);
    display.fill(Color::white, Window(xc - radius, yc, xc + radius, yc + 1));

    Serial.print("[Activity Demo] Symbol - drawn.\r");
    break;
  case 3: // symbol "\"
    display.clear(area);
    display.configureBuffer(area);
    for (int i = 0; i < length; i++)
      display.setPixel(i, i, Color::white);
    display.drawBuffer();

    Serial.print("[Activity Demo] Symbol \\ drawn.\r");

    Serial.println("[Activity Demo] Finished a cycle.");
    break;
  }

  activityState = (activityState + 1) % 4;
}
void studentIdDemo()
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

  Serial.print("[StudentID demo] Printed student ");
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

  Serial.println("[Exercise 7.1] Setup done. Type 'help' for a list of commands.");
}

void loop()
{
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("help"))
      help();
    else if (command.startsWith("clearDisplay") || command.startsWith("clear"))
      clearDisplay();
    else if (command.startsWith("runCrossDemo") || command.startsWith("cross"))
      runCrossDemo();
    else if (command.startsWith("runRotatingBarDemo") || command.startsWith("rbd"))
      runRotatingBarDemo();
    else if (command.startsWith("runStudentIdDemo") || command.startsWith("id"))
      runStudentIdDemo();
    else if (command.startsWith("stopDemo") || command.startsWith("stop"))
      stopDemo();
    else if (!command.startsWith("\n") && !command.startsWith("\r"))
      Serial.println("Unknown command. Type 'help' for a list of commands.");
  }
}
