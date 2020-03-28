
#include "lcd_menu.h"


Lcd_menu lcd_menu; 


void setup() {
  Serial.begin(9600); 
  lcd_menu.set_TidalVolume_cmd(41); //Ça se configure en décimêtre cube, pas centimètre cube. 
  lcd_menu.set_InspiPressure_cmd(21);
  lcd_menu.set_RespiratoryRate_cmd(11);
  lcd_menu.set_IERatio_cmd(21);
  lcd_menu.set_passcode(1,1,1,1);
 }

void loop() {
  lcd_menu.lcd_run();
  
  lcd_menu.set_TidalVolume_reading(lcd_menu.get_TidalVolume_cmd());
  lcd_menu.set_Inspi_pressure(lcd_menu.get_InspiPressure_cmd());
  lcd_menu.set_FiO2(lcd_menu.get_RespiratoryRate_cmd()+lcd_menu.get_IERatio_cmd());
  lcd_menu.set_Peep_pressure(lcd_menu.get_FiO2Target_cmd());

  lcd_menu.startAlarm(4);
  //Serial.print(lcd_menu.get_on_off());

  delay(50);
}
