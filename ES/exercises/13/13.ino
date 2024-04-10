const byte led = 13, button = 3;

volatile bool ledState = LOW;
bool buttonClicked = false;

void flip()
{
  ledState = !ledState;
  digitalWrite(led, ledState);
}

void check()
{
  const bool buttonState = digitalRead(button);
  if (buttonState && !buttonClicked)
  {
    flip();
    buttonClicked = true;
  }
  else if (!buttonState && buttonClicked)
  {
    buttonClicked = false;
  }
}

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  digitalWrite(led, ledState);
}

void loop()
{
  check();
  delay(10);
}