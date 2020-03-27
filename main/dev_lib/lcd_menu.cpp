#include "lcd_menu.h"

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define MaxPressure 0
#define TidalVolume 1
#define RespiratoryRate 2
#define IERatio 3
#define FiO2Target 4
using namespace std;

Lcd_menu::Lcd_menu() : lcd(8, 9, 4, 5, 6, 7)
{

  //LCD
  //int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
  //lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
  
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
    out_pc = 0; 
    out_fio2 = 0;
    tidalVolume_cmd = 0; 
    maxPressure_cmd = 0;
    respiratoryRate_cmd = 0;
    on_off = false;


  //display 
  display_cursor_pos[0][0] = 9; // The conf option 
  display_cursor_pos[1][0] = 0; 
  display_cursor_pos[0][1] = 13; // the off option
  display_cursor_pos[1][1] = 0;  
  display_cursor_state = 0;
  display_cursor_pos_size = sizeof(display_cursor_pos[0])/2; 

  //Config
  config_cursor_pos[0][0] = 11;
  config_cursor_pos[1][0] = 0;
  config_cursor_pos[0][1] = 0;
  config_cursor_pos[1][1] = 1;
  config_cursor_pos[0][2] = 0;
  config_cursor_pos[1][2] = 1;
  config_cursor_pos[0][3] = 0;
  config_cursor_pos[1][3] = 1;
  config_cursor_pos[0][4] = 0;
  config_cursor_pos[1][4] = 1;
  config_cursor_pos[0][5] = 0;
  config_cursor_pos[1][5] = 1;
  
  config_cursor_pos_size = sizeof(config_cursor_pos[0])/2; 
  config_cursor_state = 0;
  config_value[TidalVolume]=40;
  config_value[RespiratoryRate]=10;
  config_value[MaxPressure]=20;
  config_value[IERatio]=20;
  config_value[FiO2Target]=21;
  config_list=0;


  //Alarm
  alarm_name[0] = "NoAlarm  ";
  alarm_name[1] = "!Low P   ";
  alarm_name[2] = "!Hig P   ";
  alarm_name[3] = "!Dis P   ";
  alarm_name[4] = "!Low O   ";
  alarm = 0;

  state_machine_name[1] =  "Inspi    ";
  state_machine_name[2] =  "Expi     ";
  state_machine_name[3] =  "Stop     ";
  state_machine=1;
  
  
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
      return;
    case 1:
      state_display();
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
  int row = min0max100(config_cursor_state-1);

  //Refresh the display
  lcd.setCursor(0, 0);
  lcd.print("Settings");
  lcd.print("   ");
  if (on_off){
    lcd.print("Done ");}
  else {
    lcd.print("Start ");}
  
  config_parser(config_cursor_state);
 /* lcd.setCursor(0, 1);
  lcd.print(config_name[row]);
  lcd.print("");
  lcd.print(intToChar(config_value[row],2)); 
  */
  int cursor_x = config_cursor_pos[0][config_cursor_state];
  int cursor_y = config_cursor_pos[1][config_cursor_state];
  lcd.setCursor(cursor_x,cursor_y);
  lcd.cursor();

  
/**************************BUTTON LOGIC **************************/
   int button = read_LCD_buttons();
   if (button == btnDOWN){
      if (config_cursor_state == config_cursor_pos_size-1){}
      else {config_cursor_state++;}}
   
   if (button == btnUP){
      if (config_cursor_state == 0) {}
      else {
       config_cursor_state = config_cursor_state-1;}}

   if (button == btnRIGHT){
      if(config_cursor_state==0){}
      else{
        //config_value[config_cursor_state-1]=min0max100(config_value[config_cursor_state-1]+1);}}
        set_cmd_value(config_cursor_state-1,min0max100(config_value[config_cursor_state-1]+1));}}
        
   if (button == btnLEFT){
      if(config_cursor_state==0){}
      else{
        set_cmd_value(config_cursor_state-1,min0max100(config_value[config_cursor_state-1]-1));}}
        
   if (button == btnSELECT &&config_cursor_state == 0){
    on_off = true;
    state = 1;
   }
}

/*********************DISPLAY_FUNCTION*************************/
void Lcd_menu::config_parser(int index)
{

  switch (index)
  {
    case 0: 
    case 1:
      lcd.setCursor(0, 1);
      lcd.print("Inspi P. (cm) ");
      lcd.print(intToChar(config_value[MaxPressure],2)); 
      return;
    case 2: 
      lcd.setCursor(0, 1);
      lcd.print("Tidal Vol(ml)");
      lcd.print(intToChar(config_value[TidalVolume]*10,3)); 
      return;
    case 3:
      lcd.setCursor(0, 1);
      lcd.print("Respi/min     ");
      lcd.print(intToChar(config_value[RespiratoryRate],2)); 
      return;
    case 5:
      lcd.setCursor(0, 1);
      lcd.print("FiO2 Target(%)");
      lcd.print(intToChar(config_value[FiO2Target],2)); 
      return;
    case 4:
      lcd.setCursor(0, 1);
      lcd.print("I:E Ratio  1:");
      String data = intToChar(config_value[IERatio],2);
      lcd.print(data.charAt(0)); 
      lcd.print("."); 
      lcd.print(data.charAt(1)); 
      return;

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

  //Refresh the display
  lcd.setCursor(0, 0);
  if (alarm==0){
    lcd.print(state_machine_name[state_machine]);}
  else{
    lcd.print(alarm_name[alarm]);}
  
  lcd.print("Set");
  lcd.print(" ");
  lcd.print("Off");
  lcd.setCursor(0, 1);
  lcd.print("V:");
  lcd.print(intToChar(tidalVolume_reading*10,3));
  lcd.print(" ");
  lcd.print("P:");
  lcd.print(intToChar(out_pc,2));
  lcd.print(" ");
  lcd.print("O2:");
  lcd.print(intToChar(out_fio2,2));
  lcd.print(" ");
  

  //Set the cursor according to the state of the cursor
  int cursor_x = display_cursor_pos[0][display_cursor_state];
  int cursor_y = display_cursor_pos[1][display_cursor_state];
  lcd.setCursor(cursor_x,cursor_y);
  lcd.cursor();

  //Button logic
  int button = read_LCD_buttons();
  
   if (button == btnRIGHT){
      if (display_cursor_state == display_cursor_pos_size-1){
        display_cursor_state = 0;}
      else {
        display_cursor_state++;}
   }
   if (button == btnLEFT){
      if (display_cursor_state == 0){
        display_cursor_state = display_cursor_pos_size-1 ;}
      else {
        display_cursor_state = display_cursor_state-1;}}
        
   if (button == btnSELECT)
   {
      if (display_cursor_state == 0) 
      {
        state = 0;
      }
      else if (display_cursor_state == 1)
      {
        state = 0;
        on_off=false;
      }
   }

}

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
int Lcd_menu::get_TidalVolume_cmd()
{
    return config_value[TidalVolume];
}
int Lcd_menu::get_MaxPressure_cmd()
{
    return config_value[MaxPressure];
}
int Lcd_menu::get_RespiratoryRate_cmd()
{
    return config_value[RespiratoryRate];
}
int Lcd_menu::get_IERatio_cmd()
{
  return config_value[IERatio];
}
int Lcd_menu::get_Fi02Target_cmd()
{
  return  config_value[FiO2Target];
}
void  Lcd_menu::set_cmd_value(int cmd,int value)
{
  switch (cmd){
    case TidalVolume:
      set_TidalVolume_cmd(value);
      return;
    case MaxPressure:
      set_MaxPressure_cmd(value);
      return;
    case RespiratoryRate:
      set_RespiratoryRate_cmd(value);
      return;
    case IERatio:
      set_IERatio_cmd(value);
      return;
    case FiO2Target:
      set_FiO2Target_cmd(value);
      return;
  }
}
void  Lcd_menu::set_TidalVolume_cmd(int volume)
{ 
    config_value[TidalVolume] = minMax(volume, 0, 99);
}
void  Lcd_menu::set_MaxPressure_cmd(int pressure)
{
    config_value[MaxPressure] = minMax(pressure,10,40);
}
void  Lcd_menu::set_RespiratoryRate_cmd(int rate)
{
    config_value[RespiratoryRate] = minMax(rate,6,40);
}
void  Lcd_menu::set_IERatio_cmd(int ratio)
{
    config_value[IERatio] = minMax(ratio,10,40);
}
void  Lcd_menu::set_FiO2Target_cmd(int target)
{
    config_value[FiO2Target] = minMax(target,21,99);
}
/*************************************DISPLAY**************************************/
void Lcd_menu::set_TidalVolume_reading(int volume)
{
    tidalVolume_reading= volume; 
}
void Lcd_menu::set_Pressure(int pressure)
{
    out_pc = pressure;
}
void Lcd_menu::set_Fio2(int fio2)
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


int Lcd_menu::min0max100(int number)
{
  if (number < 0){
    number = 0; }
    
  if (number > 99){
    number = 99;}

  return number; 
}

int Lcd_menu::minMax(int value, int min_value, int max_value)
{
  if (value < min_value){
    value = min_value;}
  if (value > max_value){
    value = max_value;}

  return value;
  
}
