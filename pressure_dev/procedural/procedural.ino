#include "Wire.h"
#include <Arduino.h>
#include <EEPROM.h>


#define HSCDRRN400MD2A3_I2C 0x28 // each I2C object has a unique bus address, the DS1307 is 0x68
#define OUTPUT_MIN 1638.4        // 1638 counts (10% of 2^14 counts or 0x0666)
#define OUTPUT_MAX 14745.6       // 14745 counts (90% of 2^14 counts or 0x3999)
#define PRESSURE_MIN -344.738        // min is 0 for sensors that give absolute values  //<<<<<<<<<<<<<<<<<<<<<<  Modified
#define PRESSURE_MAX 344.738   // 1.6bar (I want results in bar)  //<<<<<<<<<<<<<<<<<<<<<<  Modified


void setup()
{
 Wire.begin(); // wake up I2C bus
 delay (500);
 Serial.begin(9600);
 Serial.println("Hello World!");
}


void loop()
{
 float pressure, temperature;
 //send a request

    Wire.beginTransmission(HSCDRRN400MD2A3_I2C); // "Hey, CN75 @ 0x48! Message for you"
    Wire.write(1);  // send a bit asking for register one, the data register (as specified by the pdf)
    Wire.endTransmission(); // "Thanks, goodbye..."
    // now get the data from the sensor
 
    delay (20);

    Wire.requestFrom(HSCDRRN400MD2A3_I2C, 4);
    
    if(Wire.available() != 0)
    {
      byte a     = Wire.read(); // first received byte stored here ....Example bytes one: 00011001 10000000
      byte b     = Wire.read(); // second received byte stored here ....Example bytes two: 11100111 00000000
      byte c     = Wire.read(); // third received byte stored here
      byte d     = Wire.read(); // fourth received byte stored here
      byte status1 = (a & 0xc0) >> 6;  // first 2 bits from first byte
      int bridge_data = ((a & 0x3f) << 8) + b;
    
    
     int temperature_data = ((c << 8) + (d & 0xe0)) >> 5;
    
             if ( temperature_data == 65535 ) {
                 Serial.println("err HSCDRRN400MD2A3 sensor missing");
             }
             
             if ( status1 == 1 ) {
                 Serial.println("warn command mode ");// *Command mode is used for programming the sensor. This mode should not be seen during normal operation.
                // Serial.println(status, BIN);   
             }
             
             if ( status1 == 2 ) {   
                 Serial.println("warn stale data ");  // if data has already been feched since the last measurement cycle
                // Serial.println(status, BIN);
             }   
             
             if ( status1 == 3) {
                 Serial.println("err diagnostic fault "); //When the two status bits are "11", one of the above mentioned diagnostic faults is indicated.
                // Serial.println(status, BIN);
             }
    
    
            pressure = 1.0 * (bridge_data - OUTPUT_MIN) * (PRESSURE_MAX - PRESSURE_MIN) / (OUTPUT_MAX - OUTPUT_MIN) + PRESSURE_MIN;
            pressure = pressure - 0.4;   //<<<<<<<<<<<<<<<<<<<<<<  Modified
            temperature = (temperature_data * 0.0977) - 50;
    
     
     
             Serial.print("status      ");
             Serial.println(status1, BIN);
             Serial.print("bridge_data ");
             Serial.println(bridge_data, DEC);
             Serial.print("temp_data   ");
             Serial.println(temperature_data, DEC);
             Serial.println("");
    
             Serial.print("pressure    (BAR) ");
             Serial.println(pressure);
             Serial.print("temperature (C) ");
             Serial.println(temperature);
             Serial.println("");
             
             delay (500);
    }
}
