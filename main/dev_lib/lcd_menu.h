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
        int tidalVolume_reading; 
        int out_pc; 
        int out_fio2;
      
        int tidalVolume_cmd;
        int maxPressure_cmd;
        int respiratoryRate_cmd;
        bool on_off;

        //Alarm 
        void startAlarm(int alarm_nb);
        void stopAlarm();
        int alarm = 0;
        
        void setMode(int mode);
        int mode = 0;
        
        /************************GET / SET ******************************/
        //CONFIG
        int get_TidalVolume_cmd();
        int get_MaxPressure();
        int get_RespiratoryRate();
        int get_IERatio();

        void  set_TidalVolume_cmd(int volume);
        void  set_MaxPressure_cmd(int pressure);
        void  set_RespiratoryRate_cmd(int rate);
        void  set_IERatio_cmd(int ratio); 
        void  set_cmd_value(int cmd, int value);

        //DISPLAY
        void set_TidalVolume_reading(int volume);
        void set_Pressure(int pressure);
        void set_Fio2(int fio2);

        //OTHERS
        bool get_on_off();
       

        //display
        //First value is top/down and the second one is which position, so 1 per position
        int display_cursor_pos[2][2];
        int display_cursor_pos_size; 
        int display_cursor_state;

        //config
        int config_cursor_pos[2][5];
        int config_cursor_pos_size;
        int config_cursor_state;
        int config_list;
        int config_value[4];

        void config_parser(int index);

        /********************************UTILITAIRE**********************************/
        String intToChar(int number, int nb_digit); 
        int min0max100(int number);
        int minMax(int value, int min_value, int max_value);
        
        //Config

    protected:

    private:
   

    
};

#endif // LCD_MENU_H
