#include "Wire.h"
#include "pressure.h"
#include <Arduino.h>
#include <EEPROM.h>
int count =0;

Pressure_gauge pressure_gauge(0x28); 
void setup()
{
  Serial.begin(9600);
  pressure_gauge.init();
  delay(500);
}


void loop()
{
  Serial.println(count);
  count++;
  //pressure_gauge.send();
  //delay(20);
  //pressure_gauge.read();
  //Serial.println(pressure_gauge.get_pressure());
  //Serial.println(pressure_gauge.get_temperature());
  Serial.println(pressure_gauge.calibrate());
  delay(1000);
}
