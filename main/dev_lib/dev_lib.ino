
#include "lcd_menu.h"


Lcd_menu lcd_menu; 


void setup() {
  Serial.begin(9600); 
 }

void loop() {
  lcd_menu.lcd_run();
  lcd_menu.set_TidalVolume(lcd_menu.get_TidalVolume());
  lcd_menu.set_Pressure(lcd_menu.get_MaxPressure());
  lcd_menu.set_Fio2(lcd_menu.get_RespiratoryRate()+lcd_menu.get_IERatio());

  delay(50);
}
