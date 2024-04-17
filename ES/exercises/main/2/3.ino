// pulse-width modulate led with potentiometer input

const byte pot = 9;
const byte led = 13;

void modulate()
{
  int value = analogRead(pot); // 0-1023
  int duty = value / 4;        // scale to duty (0-255) 1/4~255/1023

  analogWrite(led, duty);
}

void setup()
{
  pinMode(led, OUTPUT);
}

void loop()
{
  modulate();
  delay(100);
}