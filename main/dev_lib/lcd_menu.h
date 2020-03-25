#ifndef LCD_MENU_H
#define LCD_MENU_H
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>



class Lcd_menu
{
    public:
        Lcd_menu();
        virtual ~Lcd_menu();

        //LCD 
        LiquidCrystal lcd;

        //Boutons
        int read_LCD_buttons();
        int lcd_key;
        int adc_key_in;
        
        //Machine a état
        void lcd_run();
        int state; 
        void state_display(); 
        void state_config(); 

        //Variables
        int out_tv; 
        int out_pc; 
        int out_fio2;


        
        int in_tv;
        int in_mp;
        int in_rr;

        int get_TidalVolume();
        int get_MaxPressure();
        int get_RespiratoryRate();
        int get_IERatio();
        
        void set_TidalVolume(int volume);
        void set_Pressure(int pressure);
        void set_Fio2(int fio2);

        bool on_off;

        //display
        //First value is top/down and the second one is which position, so 1 per position
        int display_cursor_pos[2][2];
        int display_cursor_pos_size; 
        int display_cursor_state;

        //config
        int config_cursor_pos[2][5];
        int config_cursor_pos_size;
        int config_cursor_state;
        String config_name[4]; 
        int config_list;
        int config_value[4];

        //utilitaire
        String intToChar(int number, int nb_digit); 
        int min0max100(int number);
        
        //Config

    protected:

    private:
   

    
};

#endif // LCD_MENU_H
