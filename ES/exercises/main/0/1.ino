byte pin_led = 13;
bool ledState = LOW;

void flip()
{
  ledState = !ledState;
}

void setup()
{
  pinMode(pin_led, OUTPUT);
  digitalWrite(pin_led, ledState);
}

void loop()
{
  flip();
  digitalWrite(pin_led, ledState);
  delay(400);
  flip();
  digitalWrite(pin_led, ledState);
  delay(200);
}