#ifndef PRESSURE_H
#define PRESSURE_H
#include "Wire.h"
#include <Arduino.h>
#include <EEPROM.h>

#define HSCDRRN400MD2A3_I2C 0x28 // each I2C object has a unique bus address, the DS1307 is 0x68
#define OUTPUT_MIN 1638        // 1638 counts (10% of 2^14 counts or 0x0666)
#define OUTPUT_MAX 14745       // 14745 counts (90% of 2^14 counts or 0x3999)
#define PRESSURE_MIN -34.74        // min is 0 for sensors that give absolute values  
#define PRESSURE_MAX 34.74   // 1.6bar (I want results in bar)  


class Pressure_gauge
{
      public:
        Pressure_gauge(int addr);
        virtual ~Pressure_gauge();
        void init();
        void send();
        int read(); 

        int address;
        float pressure;
        float temperature;

        int get_address();
        float get_pressure();
        float get_temperature();
};
#endif // PRESSURE_H
