#ifndef SCREEN_H
#define SCREEN_H
#include <Arduino.h>
#include <string.h>
#include <Vector.h>
const int ELEMENT_COUNT_MAX =10;

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
    
};

#endif // SCREEN_H
