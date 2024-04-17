// print info to serial console: pin A9 – ADC-value: 257; input Voltage: 1.25 V, 1256 mV

const byte pin = 9;

int oldValue = 0;

void status()
{
  int value = analogRead(pin);
  if (value < oldValue + 10 && value > oldValue - 10)
  {
    return;
  }

  oldValue = value;

  float voltage = value * 3.3 / 1023;
  float voltageInmV = voltage * 1000;

  Serial.print("pin A");
  Serial.print(pin);
  Serial.print(" - ADC-value: ");
  Serial.print(value);
  Serial.print("; input voltage: ");
  Serial.print(voltage);
  Serial.print("V, ");
  Serial.print(voltageInmV);
  Serial.print("mV\n\n");
}

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  status();
  delay(100);
}