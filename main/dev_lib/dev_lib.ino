#include "screen.h"
#include "lcd_menu.h"
#define InspiPressure 0
#define TidalVolume 1
#define RespiratoryRate 2
#define IERatio 3
#define FiO2Target 4

Lcd_menu lcd_menu; 
Screen screen;


void setup() {


  Serial.begin(9600); 
  Serial.println("Hello World");
  lcd_menu.set_passcode(0,0,0,0);
  lcd_menu.lcd_run();
  
 }

void loop() {
  //screen.test();
  int button_pressed = lcd_menu.read_LCD_buttons();

  if (button_pressed!=5)
  {
    lcd_menu.lcd_run();
    lcd_menu.read_LCD_buttons();
    lcd_menu.lcd_run();
   
  }
  
  lcd_menu.set_TidalVolume_reading  (lcd_menu.get_cmd_value(TidalVolume));
  lcd_menu.set_Inspi_pressure       (lcd_menu.get_cmd_value(InspiPressure));
  lcd_menu.set_FiO2                 (lcd_menu.get_cmd_value(FiO2Target));
  lcd_menu.set_Peep_pressure        (lcd_menu.get_cmd_value(RespiratoryRate));

  delay(50);
}
