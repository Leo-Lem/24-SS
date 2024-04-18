#include <Arduino.h>
#define POT 9

static int oldValue = 0;

static void status()
{ // print info to serial console: pin A9 – ADC-value: 257; input Voltage: 1.25 V, 1256 mV
  int value = analogRead(POT);
  if (value < oldValue + 10 && value > oldValue - 10)
  {
    return;
  }

  oldValue = value;

  float voltage = value * 3.3 / 1023;
  float voltageInmV = voltage * 1000;

  Serial.print("pin A");
  Serial.print(POT);
  Serial.print(" - ADC-value: ");
  Serial.print(value);
  Serial.print("; input voltage: ");
  Serial.print(voltage);
  Serial.print("V, ");
  Serial.print(voltageInmV);
  Serial.print("mV\n\n");
}

void setup22()
{
  Serial.begin(9600);
}

void loop22()
{
  status();
  delay(100);
}