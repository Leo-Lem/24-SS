const byte led = 13, button = 3;

volatile bool ledState = LOW;

void flip()
{
  ledState = !ledState;
  digitalWrite(led, ledState);
}

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(button), flip, CHANGE);
}

void loop()
{
}