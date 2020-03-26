#include "Wire.h"
#include "pressure.h"
#include <Arduino.h>
#include <EEPROM.h>
int count =0;

#define HSCDRRN400MD2A3_I2C 0x28 // each I2C object has a unique bus address, the DS1307 is 0x68
#define OUTPUT_MIN 1638.4        // 1638 counts (10% of 2^14 counts or 0x0666)
#define OUTPUT_MAX 14745.6       // 14745 counts (90% of 2^14 counts or 0x3999)
#define PRESSURE_MIN -344.738        // min is 0 for sensors that give absolute values  //<<<<<<<<<<<<<<<<<<<<<<  Modified
#define PRESSURE_MAX 344.738   // 1.6bar (I want results in bar)  //<<<<<<<<<<<<<<<<<<<<<<  Modified

Pressure_gauge pressure_gauge; 
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
  pressure_gauge.run();
  delay(500);
}
