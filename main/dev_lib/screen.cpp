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
void Config::set_value(int value_)
{
  if(value_> max){value = max;}
  else if(value_< min){value=min;}
  else {value = value_;}
  
}
int Config::get_value()
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
void Config::set_cursor(int x, int y)
{
  cursor_x = x; 
  cursor_y = y;
}
void Config::set_displayType(int nb_digit_, int pos_comma_)
{
  nb_digit = nb_digit_; 
  pos_comma = pos_comma_;
}

String Config::create_display()
{
 int flag_comma =0;
 if (pos_comma!=0) {flag_comma= 1;}
 cursor_x = 16-nb_digit-flag_comma;
 cursor_y = 1;

 
 return " ";
}
int Config::minMax(int value_, int min_value, int max_value)
{
  if (value_ < min_value){
    value_ = min_value;}
  if (value > max_value){
    value_ = max_value;}

  return value_;
  
}
String Config::create_line()
{ 
  
  char etape1[16]={}; 
  intToChar(etape1,value,nb_digit);
 
  char etape2[16]={};
  putComma(etape2,etape1,pos_comma,nb_digit);
  
  int flag_comma=0;
  if (pos_comma >= 0)
    {flag_comma = 1;}
    
  char etape3[17]={};
  int sep = 16-nb_digit-flag_comma;
  for (int i=0; i<16; i++)
  {
    if(i<sep && i<display_name.length())
    {
      etape3[i] = display_name[i];
    }
    else if (i<sep)
    {
      etape3[i] = 32;
    }
    else
    {
      etape3[i] = etape2[i-sep];
    }
  }
  String out(etape3);
  
  return out;
  
}

void Config::intToChar(char out[], int number,int nb_digit_)
 {
    
    char line[4]={};
    sprintf(line,"%%0%dd",nb_digit_);
    sprintf(out,line, number);
    return;
 
 }

 
 void Config::putComma(char out[],char in[], int nb_comma, int nb_digit_)
 {
  int flag_comma = 0; 
  
  for (int i=0; i<nb_digit_; i++)
  {
    if(i == nb_comma){
      out[i]=46;
      flag_comma=1;
    }
      out[i+flag_comma]=in[i];
  }
  
  return;
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
