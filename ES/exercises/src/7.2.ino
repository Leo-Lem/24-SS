#include <SD.h>

#define SD_CS 12

#include "Display.h"
#include "Timer.h"

#ifdef __AVR_ATmega2560__
Display display = Display::ili9341;
#else
Display display = Display::st7735;
#endif

void help()
{
  Serial.println("help - Print a list of commands.");
  Serial.println("clearDisplay|clear - Delete the buffer and clear the display.");
  Serial.println("runCrossDemo|cross - Start the cross demo (from the example).");
  Serial.println("runRotatingBarDemo|rbd - Start the rotating bar demo (from exercise 6.4).");
  Serial.println("runStudentIdDemo|id - Start the student ID demo (from exercise 6.6).");
  Serial.println("stopDemo|stop - Stop the running demo.");
  Serial.println("listDirectory <dir name> |ls <dir name> - Lists all files in the directory specified by <dir name>.");
  Serial.println("doesFileExist <file name>|exist <file name> - Checks if the file specified by <file name> exists.");
  Serial.println("outputFileToSerial <file name>|catSer <file name> - Outputs the raw content of the file specified by <file name> to the serial monitor.");
  Serial.println("outputFileToLCD <file name>|catTFT <file name> - Outputs the (converted) content of the file specified by <file name> to the display.");
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
void listDirectory(String dirName)
{
  File dir = SD.open(dirName);
  if (!dir)
  {
    Serial.println("Directory does not exist.");
    return;
  }

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

  display.clear();

  if (fileName.endsWith(".txt"))
  {
    int x = 0, y = 0;

    int lineWidth = display.getDimensions().width;

    while (file.available())
    {
      char c = file.read();
      if (c == '\n')
      {
        x = 0, y++;
        continue;
      }
      else if (x > lineWidth / 6)
        x = 0, y++;
      else
        x++;

      display.printChar(x * 6, y * 8, c, Color::white, Color::black);
    }
  }

  // .img files
  else if (fileName.endsWith(".img"))
  {
    int x = 0, y = 0;
    while (file.available())
    {
      byte b = file.read();
      for (int i = 0; i < 8; i++)
        display.setPixel(x, y + i, (b & (1 << i)) ? Color::white : Color::black);
      x++;
      if (x == display.getDimensions().width)
      {
        x = 0;
        y += 2;
      }
    }
  }

  file.close();
}

bool crossDemoIsSetUp = false;
bool isInverted = false;

int activityState = 0;

int student = 0;
char *names[2] = {"Fritz Foerster", "Leopold Lemmermann"},
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
  SD.begin(SD_CS);

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
    else if (!command.startsWith("\n") && !command.startsWith("\r"))
      Serial.println("Unknown command. Type 'help' for a list of commands.");
  }
}