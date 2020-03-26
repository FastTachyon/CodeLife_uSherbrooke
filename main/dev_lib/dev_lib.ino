
#include "lcd_menu.h"


Lcd_menu lcd_menu; 


void setup() {
  Serial.begin(9600); 
  lcd_menu.set_TidalVolume_cmd(41); //Ça se configure en décimêtre cube, pas centimètre cube. 
  lcd_menu.set_MaxPressure_cmd(21);
  lcd_menu.set_RespiratoryRate_cmd(11);
  lcd_menu.set_IERatio_cmd(21);
 }

void loop() {
  lcd_menu.lcd_run();
  
  lcd_menu.set_TidalVolume_reading(lcd_menu.get_TidalVolume_cmd());
  lcd_menu.set_Pressure(lcd_menu.get_MaxPressure());
  lcd_menu.set_Fio2(lcd_menu.get_RespiratoryRate()+lcd_menu.get_IERatio());
  Serial.print(lcd_menu.get_on_off());

  delay(50);
}
