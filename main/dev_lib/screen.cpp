#include "screen.h"

using namespace std;

/***********************************************************************************
 * __________  _   __________________   ________    ___   __________
  / ____/ __ \/ | / / ____/  _/ ____/  / ____/ /   /   | / ___/ ___/
 / /   / / / /  |/ / /_   / // / __   / /   / /   / /| | \__ \\__ \ 
/ /___/ /_/ / /|  / __/ _/ // /_/ /  / /___/ /___/ ___ |___/ /__/ / 
\____/\____/_/ |_/_/   /___/\____/   \____/_____/_/  |_/____/____/  
                                                                    
 **********************************************************************************/
Config::Config()
{
  
}

Config::~Config()
{
  
}

void Config::set_display(String display_)
{
  display_name = display_;
}
void Config::set_value(float value_)
{
  if(value_> max){value = max;}
  else if(value_< min){value=min;}
  else {value = value_;}
  
}
float Config::get_value()
{
  return value;
}

String Config::get_line()
{
  return line;
}
void Config::set_minMax(int min_, int max_)
{
  min = min_;
  max = max_;
}

int Config::get_cursor_x()
{
  return cursor_x; 
}
int Config::get_cursor_y()
{
  return cursor_y;
}
void Config::value_pp()
{
  set_value(value+increment);
}
void Config::value_mm()
{
  set_value(value-increment);
}
    
void Config::set_increment(float increment_)
{
  increment = increment_;
}
float Config::get_increment(){
  return increment;
}

void Config::set_precision(int precision_)
{
  precision = precision_; 
}


float Config::minMax(float value_, float min_value, float max_value)
{
  if (value_ < min_value){
    value_ = min_value;}
  if (value > max_value){
    value_ = max_value;}

  return value_;
  
}
String Config::create_line()
{ 
  char step1[16]={}; 
  dtostrf(value, 0, precision, step1);
  cursor_x = 0;
  cursor_y = 1;
  String step2(step1);
  
  char step3[17]={};
  int sep = 16-step2.length();
  
  for (int i=0; i<16; i++)
  {
    if(i<sep && i<display_name.length()){
      step3[i] = display_name[i];}
    else if (i < sep){
      step3[i] = 32;}
    else{
      step3[i] = step2[i-sep];}
  }
  String out(step3);
  return out;
}

/***********************************************************************************
 * _____ __________  _____________   __   ________    ___   __________
  / ___// ____/ __ \/ ____/ ____/ | / /  / ____/ /   /   | / ___/ ___/
  \__ \/ /   / /_/ / __/ / __/ /  |/ /  / /   / /   / /| | \__ \\__ \ 
 ___/ / /___/ _, _/ /___/ /___/ /|  /  / /___/ /___/ ___ |___/ /__/ / 
/____/\____/_/ |_/_____/_____/_/ |_/   \____/_____/_/  |_/____/____/  
                                                                      
 ************************************************************************************/
Screen::Screen() 
{
  element_location_x.setStorage(storage_array_x);
  element_location_y.setStorage(storage_array_y);
}

Screen::~Screen()
{
    //dtor
}

void Screen::test()
{
  Serial.println("Hello world");
  element_location_x.push_back(1);
  element_location_y.push_back(1);
  Serial.println(element_location_x[0]);
}
void Screen::add_element_location(int x, int y)
{
  element_location_x.push_back(x);
  element_location_y.push_back(y);
}
int Screen::get_element_location_x(int x)
{
  return element_location_x[x];
  }
int Screen::get_element_location_y(int y)
{
  return element_location_y[y];
}
void Screen::set_cursor_position(int position)
{
  cursor_position = position; 
}
int Screen::get_cursor_position()
{
  return cursor_position;
}
int Screen::get_element_size()
{
  return element_location_x.size();
}

void Screen::next_element(int option=0)
{
  if ((cursor_position == element_location_x.size()-1))
  {
    if(option==0)
    {
      cursor_position=0;
    }
  }
  else
  {
    cursor_position = cursor_position +1;
  }
  
}
void Screen::previous_element(int option=0)
{
  if (cursor_position == 0)
  {
    if(option==0)
    {
      cursor_position = element_location_x.size()-1;
    }

  }
  else
  {
    cursor_position = cursor_position -1;
  }
}
