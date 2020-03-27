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
  pressure_gauge.calibrate();
}


void loop()
{
  count++;
  //pressure_gauge.offset_pressure =0;
  //Serial.print("Offest = ");
  //Serial.println(pressure_gauge.calibrate());
  pressure_gauge.send();
  pressure_gauge.read();
  Serial.println(pressure_gauge.get_pressure());
  //Serial.println(pressure_gauge.get_temperature());
  delay(30);
}
