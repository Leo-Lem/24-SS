char ch_buffer[80] = "";
float pi_fl = 3.14159265359;

void ftoa(float num, char *res, int numDecPlaces)
{
  // Handle negative numbers
  if (num < 0)
  {
    *res++ = '-';
    num = -num;
  }

  // Extract integer part
  int int_part = (int)num;
  itoa(int_part, res, 10);
  while (*res != '\0')
    res++;

  // Add decimal point
  *res++ = '.';

  // Extract decimal part
  float dec_part = num - int_part;
  for (int i = 0; i < numDecPlaces; i++)
  {
    dec_part *= 10;
    int digit = (int)dec_part;
    *res++ = '0' + digit;
    dec_part -= digit;
  }

  *res = '\0';
}

void print_float(float num, int numDecPlaces)
{
  char buffer[80];
  ftoa(num, buffer, numDecPlaces);
  Serial.print(buffer);
}

void setup()
{
  Serial.begin(9600);

  Serial.println("Test print Pi\n");
  Serial.print("print Pi using print_float: ");
  print_float(pi_fl, 3);
  Serial.println(" ");

  Serial.print("print Pi using ftoa: ");
  ftoa(pi_fl, ch_buffer, 5);
  Serial.println(ch_buffer);

  Serial.print("\ncross check: Serial.println(pi_ch, 5):\t");
  Serial.println(pi_fl, 5);
}
void loop()
{
}