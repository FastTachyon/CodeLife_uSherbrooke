#include <EEPROM.h>

void setup() 
{
  Serial.begin(9600);
  Serial.println("Hello World");

  int value = EEPROM.read(0);
  Serial.println(value);
  value++;
  EEPROM.write(0,value);

}

void loop() {
  // put your main code here, to run repeatedly:

}
