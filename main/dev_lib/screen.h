#ifndef SCREEN_H
#define SCREEN_H
#include <Arduino.h>
#include <string.h>
#include <Vector.h>
const int ELEMENT_COUNT_MAX =10;

class Config
{
  public:
    Config();
    virtual ~Config();
    
    void set_display(String display_);
    void set_value(float value);
    float get_value();
    String get_line();
    int get_cursor_x();
    int get_cursor_y();

    void set_minMax(int min_, int max_);
    void set_precision(int precision_);
    void set_increment(float increment_);
    float get_increment();

    void value_pp();
    void value_mm();
    
    String create_line();
    
  private:
    String display_name; 
    String line;
    float value=0; 
    int min=0;
    int max=0; 
    int display_type; 
    int cursor_x;
    int cursor_y; 
    int precision;
    float increment=1;
    float minMax(float value_, float min_value, float max_value);


    void putComma(char out[],char in[], int nb_comma, int nb_digit_);
    void intToChar(char out[], float number,int nb_digit_);
 
};
class Screen
{
    public:
        //Constructeur, initilise beaucoup de variables
        Screen();
        virtual ~Screen();
        void test();

        void add_element_location(int x, int y); 
        int get_element_location_x(int x);
        int get_element_location_y(int y);
        void set_cursor_position(int position);
        int get_cursor_position();
        int get_element_size();
        
        void next_element(int option=0);
        void previous_element(int option=0);
    protected:

    private:
       int cursor_position;
       int position_size; 
       int storage_array_x[ELEMENT_COUNT_MAX];
       int storage_array_y[ELEMENT_COUNT_MAX];
       Vector<int> element_location_x;
       Vector<int> element_location_y;
       String intToChar(int number,int nb_digit_);
       int minMax(int value, int min_value, int max_value);
    
};

#endif // SCREEN_H
