#include <GOTStateMachine.h>
#include <LiquidCrystal.h>
#include "lcd_menu.h"

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int button = 5; 

Lcd_menu lcd_menu; 

//***** State Machine *****
GOTStateMachine stateMachine(50); // execute every 50 milliseconds
void initialization();
void stopped();
void inflate();
void deflate();
void calibration();
void update_reading();



void setup() {

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, World!");
  Serial.begin(9600);
  
  
 
  
  stateMachine.setStartState(initialization); // Initialise state machine
}

void loop() {
  
  stateMachine.execute();             // process the states as required
}


//***** States *****
void initialization()
{
  update_reading();
  lcd.setCursor(0, 0);
  lcd.print("Init");
 
  if (stateMachine.isDelayComplete(1000)) 
  {
    stateMachine.changeState(stopped);
    lcd.clear();
  }
 
}

void stopped()
{
   update_reading();
   lcd.setCursor(0, 0);
   lcd.print("Stopped    ");

    
    button = (read_LCD_buttons());
    if (button == btnLEFT)
   {
      stateMachine.changeState(inflate);
      lcd.clear();
   }

}

void inflate()
{
  update_reading();
  lcd.setCursor(0, 0);
  lcd.print("Inflate    ");
  
   // State logic

  lcd.setCursor(0, 1);
  lcd.print(button);

  
  // Transitions
  if (stateMachine.isDelayComplete(1000)) {

    stateMachine.changeState(deflate);
    return;
  }

   if (button == btnRIGHT)
   {
      stateMachine.changeState(stopped);
      lcd.clear();
   }
 }

void deflate() 
{
  update_reading();
  lcd.setCursor(0, 0);
 
  lcd.print("Deflate");

  lcd.setCursor(0, 1);
  lcd.print(button);
  
  // Transitions
  if (stateMachine.isDelayComplete(1000)) 
  {
    stateMachine.changeState(inflate);
    lcd.clear();
    return;
  }

   if (button == btnRIGHT)
   {
      stateMachine.changeState(stopped);
      lcd.clear();
   }
}

void calibration()
{
  lcd.setCursor(0, 0);
  lcd.print("Calibration");
  update_reading();
}


void update_reading()
{
   button = (read_LCD_buttons());
}


int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; //5 We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT; //0 right
 if (adc_key_in < 250)  return btnUP;  //1 up
 if (adc_key_in < 450)  return btnDOWN; //2 down
 if (adc_key_in < 650)  return btnLEFT;//3 left
 if (adc_key_in < 850)  return btnSELECT; //4 select

 // For V1.0 comment the other threshold and use the one below:


 return btnNONE;  // when all others fail, return this...
}
