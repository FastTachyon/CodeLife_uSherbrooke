#include "screen.h"


using namespace std;

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
