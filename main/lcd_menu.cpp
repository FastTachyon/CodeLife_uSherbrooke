#include "lcd_menu.h"


using namespace std;

Lcd_menu::Lcd_menu() : lcd(8, 9, 4, 5, 6, 7)
{
  config[InspiPressure].set_display("PI Max (cm)");
  config[InspiPressure].set_minMax(10,40);
  config[InspiPressure].set_value(20);
  config[InspiPressure].set_precision(0);

  config[TidalVolume].set_display("Tidal Vol(ml)");
  config[TidalVolume].set_minMax(0,990);
  config[TidalVolume].set_value(400);
  config[TidalVolume].set_precision(0);
  config[TidalVolume].set_increment(10.);

  config[RespiratoryRate].set_display("Respi/min");
  config[RespiratoryRate].set_minMax(6,40);
  config[RespiratoryRate].set_value(10);
  config[RespiratoryRate].set_precision(0);

  config[IERatio].set_display("I:E Ratio");
  config[IERatio].set_minMax(1,4);
  config[IERatio].set_value(2.1);
  config[IERatio].set_precision(1);
  config[IERatio].set_increment(0.1);

  config[FiO2Target].set_display("FiO2 Target(%)");
  config[FiO2Target].set_minMax(21,99);
  config[FiO2Target].set_value(21);
  config[FiO2Target].set_precision(0);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, World!");
  
  //Boutons
  lcd_key     = 0;
  adc_key_in  = 0;

  //Machine à état
  state =0;

  //variables
  tidalVolume_reading = 0; 
  out_inspi_pressure = 0; 
  out_peep_pressure = 0; 
  out_fio2 = 0;

  on_off = false;

  //display 
  display.add_element_location(9,1);
  display.add_element_location(13,1);

  //Config
  settings.add_element_location(11,0);
  settings.add_element_location(0,1);
  settings.add_element_location(0,1);
  settings.add_element_location(0,1);
  settings.add_element_location(0,1);
  settings.add_element_location(0,1);

  //Security
  security.add_element_location(6,1);
  security.add_element_location(7,1);
  security.add_element_location(8,1);
  security.add_element_location(9,1);
  security.add_element_location(12,1);

  
  //Alarm
  alarm_name[0] = "NoAlarm  ";
  alarm_name[1] = "!Low P   ";
  alarm_name[2] = "!Hig P   ";
  alarm_name[3] = "!Dis P   ";
  alarm_name[4] = "!Low O   ";

  state_machine_name[1] =  "Inspi    ";
  state_machine_name[2] =  "Expi     ";
  state_machine_name[3] =  "Stop     ";
  state_machine=1;
  
  readEEPROM();
}

Lcd_menu::~Lcd_menu()
{
    //dtor
}

void Lcd_menu::lcd_run()
{
  //Serial.print(on_off);
  switch(state)
  {
    case 0:
      state_config();
      reset_passcode_try();
      return;
    case 1:
      state_display();
      reset_passcode_try();
      return;
    case 11:
    case 12:
    case 13:
      state_security();
      return;
     
      
  }
}
//************************Alarm*******************************/
void Lcd_menu::startAlarm(int alarm_nb)
{
  alarm=alarm_nb; 
}
void Lcd_menu::stopAlarm()
{
  alarm=0;
}
void Lcd_menu::setStateMachine(int state)
{
  state_machine = state;
}
/**********************************************************************
   _____ __        __          ______            _____      
  / ___// /_____ _/ /____     / ____/___  ____  / __(_)___ _
  \__ \/ __/ __ `/ __/ _ \   / /   / __ \/ __ \/ /_/ / __ `/
 ___/ / /_/ /_/ / /_/  __/  / /___/ /_/ / / / / __/ / /_/ / 
/____/\__/\__,_/\__/\___/   \____/\____/_/ /_/_/ /_/\__, /  
                                                   /____/   
/**********************************************************************/                                                 
void Lcd_menu::state_config() //State 1
{
  int config_cursor = settings.get_cursor_position();
  int row = minMax(config_cursor-1,0,100);

  //Refresh the display
  lcd.setCursor(0, 0);
  lcd.print("Settings");
  lcd.print("   ");
  if (on_off){
    lcd.print("Done ");}
  else {
    lcd.print("Start ");}

  lcd.setCursor(0, 1);
  lcd.print(config[row].create_line());
  
  int cursor_x; int cursor_y; 
  
  if(config_cursor ==0)
  {
    cursor_x = settings.get_element_location_x(config_cursor);
    cursor_y = settings.get_element_location_y(config_cursor); 
  }
  else
  {
    cursor_x = config[row].get_cursor_x();
    cursor_y = 1;
  }
 
  lcd.setCursor(cursor_x,cursor_y);
  lcd.cursor();
 
  
/**************************BUTTON LOGIC **************************/
   //int button = read_LCD_buttons();
   if (button_pressed == btnDOWN)
   {
      settings.next_element(1);
   }
   
   if (button_pressed == btnUP)
   {
      settings.previous_element(1);
   }

   if (button_pressed == btnRIGHT){
      if(config_cursor==0){}
      else{
        config[row].value_pp();}}
        
   if (button_pressed == btnLEFT){
      if(config_cursor==0){}
      else{
        config[row].value_mm();}}
        
   if (button_pressed == btnSELECT &&config_cursor == 0){
    state = 11;
   }
}

/******************************************************************
   _____ __        __          ____  _            __           
  / ___// /_____ _/ /____     / __ \(_)________  / /___ ___  __
  \__ \/ __/ __ `/ __/ _ \   / / / / / ___/ __ \/ / __ `/ / / /
 ___/ / /_/ /_/ / /_/  __/  / /_/ / (__  ) /_/ / / /_/ / /_/ / 
/____/\__/\__,_/\__/\___/  /_____/_/____/ .___/_/\__,_/\__, /  
                                       /_/            /____/   
******************************************************************/                                       
void Lcd_menu::state_display() //State 1
{
  int display_cursor = display.get_cursor_position();
  //Refresh the display
  lcd.setCursor(0, 1);
  if (alarm==0){
    lcd.print(state_machine_name[state_machine]);}
  else{
    lcd.print(alarm_name[alarm]);}
  
  lcd.print("Set");
  lcd.print(" ");
  lcd.print("Off");
  lcd.setCursor(0, 0);
  lcd.print(intToChar(tidalVolume_reading,3));
  lcd.print("  ");
  lcd.print(intToChar(out_inspi_pressure,2));
  lcd.print("   ");
  lcd.print(intToChar(out_peep_pressure,2));
  lcd.print("  ");
  lcd.print(intToChar(out_fio2,2));
 
  

  //Set the cursor according to the state of the cursor
  int cursor_x = display.get_element_location_x(display_cursor);
  int cursor_y = display.get_element_location_y(display_cursor);
  lcd.setCursor(cursor_x,cursor_y);
  lcd.cursor();

  //Button logic
  //int button = read_LCD_buttons();
  
   if (button_pressed == btnRIGHT){
    display.next_element();}
    
   if (button_pressed == btnLEFT){
    display.previous_element();}
        
   if (button_pressed == btnSELECT)
   {
      if (display_cursor == 0) 
      {
        state = 12;
      }
      else if (display_cursor == 1)
      {
        state = 13;
        //on_off=false;
      }
   }

}
/**************************************************************************************
 * _____ __        __          _____                      _ __       
  / ___// /_____ _/ /____     / ___/___  _______  _______(_) /___  __
  \__ \/ __/ __ `/ __/ _ \    \__ \/ _ \/ ___/ / / / ___/ / __/ / / /
 ___/ / /_/ /_/ / /_/  __/   ___/ /  __/ /__/ /_/ / /  / / /_/ /_/ / 
/____/\__/\__,_/\__/\___/   /____/\___/\___/\__,_/_/  /_/\__/\__, /  
                                                            /____/                                    
 **************************************************************************************/

 void Lcd_menu::state_security()
 {
  int security_cursor = security.get_cursor_position();
  lcd.setCursor(0, 0);
  lcd.print(" Enter Passcode ");
  lcd.setCursor(0, 1);
  lcd.print("      ");
  lcd.print(passcode_try[0]);
  lcd.print(passcode_try[1]);
  lcd.print(passcode_try[2]);
  lcd.print(passcode_try[3]);
  lcd.print("  Quit");

  
  int cursor_x = security.get_element_location_x(security_cursor);
  int cursor_y = security.get_element_location_y(security_cursor);
  lcd.setCursor(cursor_x,cursor_y);
  lcd.cursor(); 
  

    //Button logic
  //int button = read_LCD_buttons();
  
   if (button_pressed == btnRIGHT){
      security.next_element();

   }
   if (button_pressed == btnLEFT){
      security.previous_element();
   }


   if (security_cursor != 4) 
   {
     if ((button_pressed  == btnUP) && (passcode_try[security_cursor] != 9))
     {
        passcode_try[security_cursor] = (passcode_try[security_cursor]+1);
     }
  
     if ((button_pressed  == btnDOWN) && (passcode_try[security_cursor] != 0))
     {
        passcode_try[security_cursor] = (passcode_try[security_cursor]-1)%10;
     }
  
     if (button_pressed == btnSELECT)
     {
        for(int i=0; i<4; i++)
        {
          if (passcode[i] != passcode_try[i])
          {
            return;
          }
         
         }
         if (state == 11)
          {
            state = 1;
            writeEEPROM();
            on_off=true; 
          }
          else if (state ==12)
          {
            state =0;
          }
          else if (state ==13)
          {
            state =0;
            on_off=false;
          }
      }
    }
    else 
    {
      if(button_pressed ==btnSELECT)
      {
        if (state == 11)
        {
          state = 0;
        }
        if (state == 12 or state == 13)
        {
          state =1;
        }
      }
    }
    return;
 }
 void Lcd_menu::reset_passcode_try()
 {
    for(int i=0; i<4;i++)
    {
      passcode_try[i] =0;
    }
 }

 void Lcd_menu::set_passcode(int unit_0, int unit_1, int unit_2, int unit_3)
 {
  passcode[0] = unit_0; 
  passcode[1] = unit_1;
  passcode[2] = unit_2;
  passcode[3] = unit_3;
 }

 /*********************************************************************************     
    ________________  ____  ____  __  ___
   / ____/ ____/ __ \/ __ \/ __ \/  |/  /
  / __/ / __/ / /_/ / /_/ / / / / /|_/ / 
 / /___/ /___/ ____/ _, _/ /_/ / /  / /  
/_____/_____/_/   /_/ |_|\____/_/  /_/   
                                         
 *********************************************************************************/
void Lcd_menu::writeEEPROM()
{
  for(int i; i <NB_CONFIG;i++)
  {
      EEPROM.write(i,config[i].get_value()/config[i].get_increment());
  }

}
void Lcd_menu::readEEPROM()
{
  for(int i; i < NB_CONFIG;i++)
  {
    config[i].set_value(EEPROM.read(i)*config[i].get_increment());
  }
}
        
 /**********************************************************************************
    ____        __  __                  
   / __ )__  __/ /_/ /_____  ____  _____
  / __  / / / / __/ __/ __ \/ __ \/ ___/
 / /_/ / /_/ / /_/ /_/ /_/ / / / (__  ) 
/_____/\__,_/\__/\__/\____/_/ /_/____/  
                                        
  ***********************************************************************************/
int Lcd_menu::read_LCD_buttons()
{
   adc_key_in = analogRead(0);      // read the value from the sensor
   // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
   // we add approx 50 to those values and check to see if we are close
  int return_value = 0; 
  static int old_value = 0;
   if (adc_key_in > 1000) { return_value = 5;} //5 We make this the 1st option for speed reasons since it will be the most likely result
   // For V1.1 us this threshold
   if (adc_key_in < 90)   { return_value = 0;} //0 right
   else if (adc_key_in < 250)  { return_value = 1;}  //1 up
   else if (adc_key_in < 450)  { return_value = 2;} //2 down
   else if (adc_key_in < 650)  { return_value = 3;}//3 left
   else if (adc_key_in < 850)  { return_value = 4;} //4 select
  if (return_value == old_value)
  {
    return_value = 5; 
  }
  else
  {
    old_value = return_value; 
  }
   button_pressed=return_value;
   return return_value;  // when all others fail, return this...
}


/*************************************************************************************/
/* __________________      __   _____ ____________
  / ____/ ____/_  __/    _/_/  / ___// ____/_  __/
 / / __/ __/   / /     _/_/    \__ \/ __/   / /   
/ /_/ / /___  / /    _/_/     ___/ / /___  / /    
\____/_____/ /_/    /_/      /____/_____/ /_/     
*/
/*************************************************************************************/   
float Lcd_menu::get_cmd_value(int cmd_)
{
  return config[cmd_].get_value();
}
                                           
float Lcd_menu::get_TidalVolume_cmd()
{
    return config[TidalVolume].get_value();
}
float Lcd_menu::get_InspiPressure_cmd()
{
    return config[InspiPressure].get_value();
}
float Lcd_menu::get_RespiratoryRate_cmd()
{
    return config[RespiratoryRate].get_value();
}
float Lcd_menu::get_IERatio_cmd()
{
  return config[IERatio].get_value();
}
float Lcd_menu::get_FiO2Target_cmd()
{
  return  config[FiO2Target].get_value();
}

/*********************************SET CMD**********************************/
void  Lcd_menu::set_cmd_value(int cmd,float value)
{
  config[cmd].set_value(value);
}
void  Lcd_menu::set_TidalVolume_cmd(float volume)
{ 
    config[TidalVolume].set_value(volume);
}
void  Lcd_menu::set_InspiPressure_cmd(float pressure)
{
    config[InspiPressure].set_value(pressure);
}
void  Lcd_menu::set_RespiratoryRate_cmd(float rate)
{
    config[RespiratoryRate].set_value(rate);
}
void  Lcd_menu::set_IERatio_cmd(float ratio)
{
    config[IERatio].set_value(ratio);
}
void  Lcd_menu::set_FiO2Target_cmd(float target)
{
    config[FiO2Target].set_value(target);
}
/*************************************DISPLAY**************************************/
void Lcd_menu::set_TidalVolume_reading(int volume)
{
    tidalVolume_reading= volume; 
}
void Lcd_menu::set_Inspi_pressure(int pressure)
{
    out_inspi_pressure = pressure;
}
void Lcd_menu::set_Peep_pressure(int pressure)
{
    out_peep_pressure = pressure;
}
void Lcd_menu::set_FiO2(int fio2)
{
    out_fio2 = fio2;
}

/*************************************OTHERS***************************************/
bool Lcd_menu::get_on_off()
{
  return on_off;
}

/************************************************************************
  ______            __    
 /_  __/___  ____  / /____
  / / / __ \/ __ \/ / ___/
 / / / /_/ / /_/ / (__  ) 
/_/  \____/\____/_/____/  
                               
 **********************************************************************/
//Function who return int but in char with 2 or 3 digit, padded with 0. 
 String Lcd_menu::intToChar(int number,int nb_digit)
 {
    if (nb_digit == 2)
    {
      char out_string[2];
      sprintf(out_string,"%02d", number);
      return out_string;
    }
    if (nb_digit == 3)
    {
      char out_string[3];
      sprintf(out_string,"%03d", number);
      return out_string;
    }
    return;
    
 }

int Lcd_menu::minMax(int value, int min_value, int max_value)
{
  if (value < min_value){
    value = min_value;}
  if (value > max_value){
    value = max_value;}

  return value;
  
}
