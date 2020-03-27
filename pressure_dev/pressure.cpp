#include "pressure.h"


Pressure_gauge::Pressure_gauge(int addr)
{
  address = addr;
  pressure = 0;
  temperature =0;
  offset_pressure=0;
}

Pressure_gauge::~Pressure_gauge(){}


void Pressure_gauge::init()
{
   Wire.begin(); // wake up I2C bus
}
float Pressure_gauge::calibrate()
{
    float sum=0;
    int nb = 10;
    for(int i=0;i<nb;i++)
    {
      send();
      delay(20);
      read();
      sum = sum+ pressure; 
    }
    offset_pressure = sum/nb;

    return offset_pressure; 
}
void Pressure_gauge::send()
{
  
  //send a request
    Wire.beginTransmission(address); // "Hey, CN75 @ 0x48! Message for you"
    Wire.write(1);  // send a bit asking for register one, the data register (as specified by the pdf)
    Wire.endTransmission(); // "Thanks, goodbye..."
    // now get the data from the sensor
}
int Pressure_gauge::read()
{
    
    Wire.requestFrom(address, 4);
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
                 //Serial.println("err HSCDRRN400MD2A3 sensor missing");
             }
             
             if ( status1 == 1 ) {
                // Serial.println("warn command mode ");// *Command mode is used for programming the sensor. This mode should not be seen during normal operation.
                // Serial.println(status, BIN);   
             }
             
             if ( status1 == 2 ) {   
                 //Serial.println("warn stale data ");  // if data has already been feched since the last measurement cycle
                // Serial.println(status, BIN);
             }   
             
             if ( status1 == 3) {
                 //Serial.println("err diagnostic fault "); //When the two status bits are "11", one of the above mentioned diagnostic faults is indicated.
                // Serial.println(status, BIN);
             }
  
  
            pressure = 1.0 * (bridge_data - OUTPUT_MIN) * (PRESSURE_MAX - PRESSURE_MIN) / (OUTPUT_MAX - OUTPUT_MIN) + PRESSURE_MIN;
            pressure = pressure - offset_pressure;  
            temperature = (temperature_data * 0.0977) - 50;
  
        return status1; 
        }
}
void Pressure_gauge::set_offset_pressure(int offset)
{
    
}
int Pressure_gauge::get_address()
{
    return address; 
}
float Pressure_gauge::get_pressure()
{
    return pressure;
}
float Pressure_gauge::get_temperature()
{
    return temperature;
}
