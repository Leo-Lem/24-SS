// Entwicklung eines String-Parsers
// Entwickeln Sie einen String-Parser, welcher ein über das Eingabefeld des seriellen Monitors ab- geschicktes Kommando entgegennimmt und dieses auf Korrektheit überprüft. Bei einem kor- rekt erkannten Kommando veranlassen Sie die Ausführung der Funktion, andernfalls geben Sie, soweit möglich, eine möglichst aussagekräftige Fehlermeldung auf der seriellen Konsole aus. Der Befehlssatz soll folgende Kommandos enthalten:
// • help()
// Listet die gültigen Befehle mit Angabe von Information zur Nutzung der Befehle in der Ausgabe des seriellen Monitors auf.
// • LEDon()
// schaltet die LED mit dem voreingestellten Helligkeitswert ein.
// • LEDoff()
// schaltet die LED aus
// • illuminance(value)
// Lichtintensität (value) der LED in [%]
// Die Namen der Befehle dürfen Sie, insbesondere für Proteus, gerne (tipp)ökonomischer wählen.
// ACHTUNG: Vergessen Sie bitte nicht, die Benutzereingabe auf Korrektheit zu überprüfen! Für erkannte Eingabefehler soll ein „sinnvolles Feedback“ ausgegeben werden. Berücksichtigen Sie bei der Entwicklung des Parsers eine leichte Erweiterbarkeit des Befehlssatzes für spätere Auf- gaben.
// Erweitern Sie die Aufgabe 2.3 um die zusätzliche Steuerung der LED über die serielle Konsole mit obigen Kommandos. Es soll parallel die Steuerung über Taster und Poti erhalten bleiben.

const byte led = 13;

void setLED(int percent)
{
  analogWrite(led, percent * 255 / 100);
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
  pinMode(led, OUTPUT);
}

void loop()
{
  if (Serial.available())
  {
    String str = Serial.readStringUntil('\n');
    parse(str);
  }
}
