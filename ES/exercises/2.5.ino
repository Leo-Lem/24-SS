#include <Arduino.h>

#define LED 13

void setLED(int percent)
{
  analogWrite(LED, percent * 255 / 100);
}

void help()
{
  Serial.println(
      "help - list available commands\nledon - turn LED on\nledoff - turn LED off\nilluminance <value> - set LED brightness in %");
}

void ledon()
{
  setLED(100);
  Serial.println("LED is on");
}

void ledoff()
{
  setLED(0);
  Serial.println("LED is off");
}

void illuminance(int percent)
{
  if (percent >= 0 && percent <= 100)
  {
    setLED(percent);
    Serial.print("LED brightness set to ");
    Serial.print(percent);
    Serial.println("%");
  }
  else
  {
    Serial.println("Invalid value (percentage must be between 0 and 100)");
  }
}

void parse(String str)
{
  if (str == "help")
  {
    help();
  }
  else if (str == "ledon")
  {
    ledon();
  }
  else if (str == "ledoff")
  {
    ledoff();
  }
  else if (str.startsWith("illuminance"))
  {
    int value = str.substring(12).toInt();
    if (value == 0 && str[12] != '0')
    {
      Serial.println("Invalid value (percentage must be a number)");
      return;
    }

    illuminance(value);
  }
  else
  {
    Serial.println("Invalid command. Type 'help' for a list of available commands.");
  }
}

// arduino setup

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop()
{
  if (Serial.available())
  {
    String str = Serial.readStringUntil('\n');
    parse(str);
  }
}
