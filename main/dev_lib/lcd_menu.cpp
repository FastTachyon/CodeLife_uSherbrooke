#include "lcd_menu.h"

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define TidalVolume 0
#define RespiratoryRate 1
#define MaxPressure 2
#define IERatio 3

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
    out_tv = 0; 
    out_pc = 0; 
    out_fio2 = 0;
    in_tv = 0; 
    in_mp = 0;
    in_rr = 0;
    on_off = false;


  //display 
  display_cursor_pos[0][0] = 7; // The conf option 
  display_cursor_pos[1][0] = 0; 
  display_cursor_pos[0][1] = 13; // the off option
  display_cursor_pos[1][1] = 0;  
  display_cursor_state = 0;
  display_cursor_pos_size = sizeof(display_cursor_pos[0])/2; 

  //Config
  config_cursor_pos[0][0] = 11;
  config_cursor_pos[1][0] = 0;
  config_cursor_pos[0][1] = 14;
  config_cursor_pos[1][1] = 1;
  config_cursor_pos[0][2] = 14;
  config_cursor_pos[1][2] = 1;
  config_cursor_pos[0][3] = 14;
  config_cursor_pos[1][3] = 1;
  config_cursor_pos[0][4] = 14;
  config_cursor_pos[1][4] = 1;
  
  config_cursor_pos_size = sizeof(config_cursor_pos[0])/2; 
  config_cursor_state = 0;
  config_name[TidalVolume]="Tidal Volume"; 
  config_name[RespiratoryRate]="Respi. Rate "; 
  config_name[MaxPressure]="Max Pressure"; 
  config_name[IERatio]="I/E Ratio   "; 
  config_value[0]=0;
  config_value[1]=0;
  config_value[2]=0;
  config_value[3]=0;
  config_list=0;
}

Lcd_menu::~Lcd_menu()
{
    //dtor
}

void Lcd_menu::lcd_run()
{
  Serial.print(on_off);
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

void Lcd_menu::state_config() //State 1
{
  int row = min0max100(config_cursor_state-1);

  //Refresh the display
  lcd.setCursor(0, 0);
  lcd.print("Conf");
  lcd.print("       ");
  if (on_off){
    lcd.print("Done ");}
  else {
    lcd.print("Start ");}
 
  lcd.setCursor(0, 1);
  lcd.print(config_name[row]);
  lcd.print("  ");
  lcd.print(intToChar(config_value[row],2)); 
  
  int cursor_x = config_cursor_pos[0][config_cursor_state];
  int cursor_y = config_cursor_pos[1][config_cursor_state];
  lcd.setCursor(cursor_x,cursor_y);
  lcd.cursor();

  
   //Button logic
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
        config_value[config_cursor_state-1]=min0max100(config_value[config_cursor_state-1]+1);}}
   if (button == btnLEFT){
      if(config_cursor_state==0){}
      else{
        config_value[config_cursor_state-1]=min0max100(config_value[config_cursor_state-1]-1);}}
   if (button == btnSELECT &&config_cursor_state == 0){
    on_off = true;
    state = 1;
   }
}

void Lcd_menu::state_display() //State 1
{

  //Refresh the display
  lcd.setCursor(0, 0);
  lcd.print("Mode");
  lcd.print("   ");
  lcd.print("Conf");
  lcd.print("  ");
  lcd.print("Off");
  lcd.setCursor(0, 1);
  lcd.print("V:");
  lcd.print(intToChar(out_tv,2));
  lcd.print(" ");
  lcd.print("P:");
  lcd.print(intToChar(out_pc,2));
  lcd.print(" ");
  lcd.print("02:");
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

int Lcd_menu::get_TidalVolume()
{
    return config_value[TidalVolume];
}
int Lcd_menu::get_MaxPressure()
{
    return config_value[MaxPressure];
}
int Lcd_menu::get_RespiratoryRate()
{
    return config_value[RespiratoryRate];
}
int Lcd_menu::get_IERatio()
{
  return config_value[IERatio];
}


void Lcd_menu::set_TidalVolume(int volume)
{
    out_tv = volume; 
}
void Lcd_menu::set_Pressure(int pressure)
{
    out_pc = pressure;
}
void Lcd_menu::set_Fio2(int fio2)
{
  out_fio2 = fio2;
}

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
