#ifndef LCD_MENU_H
#define LCD_MENU_H
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>
#include "screen.h"



class Lcd_menu
{
    public:
        //Constructeur, initilise beaucoup de variables
        Lcd_menu();
        Screen security;
        Screen settings;
        virtual ~Lcd_menu();

        //À rouler à tous les 20-100ms
        void lcd_run();
        
        //Commande pour savoir si le système doit être à on ou off
        bool get_on_off();

        //Informer le lcd d'une alarme et de l'état de la machine (inspi, expi) 
        void startAlarm(int alarm_nb);
        void stopAlarm();
        void setStateMachine(int state);

        //Récupérer les variables de controles
        int get_TidalVolume_cmd();
        int get_InspiPressure_cmd();
        int get_RespiratoryRate_cmd();
        int get_IERatio_cmd();
        int get_FiO2Target_cmd();

        //Mettre les valeurs par défauts des variables de controles
        void  set_TidalVolume_cmd(int volume);
        void  set_InspiPressure_cmd(int pressure);
        void  set_RespiratoryRate_cmd(int rate);
        void  set_IERatio_cmd(int ratio); 
        void  set_FiO2Target_cmd(int target);

        //Informer le lcd sur les informations à afficher
        void set_TidalVolume_reading(int volume);
        void set_Inspi_pressure(int inspi_p);
        void set_Peep_pressure(int inspi_p);
        void set_FiO2(int fio2);

        void set_passcode(int unit_0, int unit_1, int unit_2, int unit_3);

    protected:

    private:
        //LCD 
        LiquidCrystal lcd;

        //Boutons
        int read_LCD_buttons();
        int lcd_key;
        int adc_key_in;
        
        //State machine

        int state; 
        void state_display(); 
        void state_config(); 
        void state_security();
        
        //Value and readings
        int tidalVolume_reading; 
        int out_inspi_pressure; 
        int out_peep_pressure;
        int out_fio2;
      
        bool on_off;


        void  set_cmd_value(int cmd, int value);
        
        //Alarm and mode
        String alarm_name[10]={""};
        int alarm = 0;
        
        String state_machine_name[10]={""};
        int state_machine = 0;
            
        //display
        //First value is top/down and the second one is which position, so 1 per position
        int display_cursor_pos[2][2];
        int display_cursor_pos_size; 
        int display_cursor_state;

        //config
        int config_cursor_pos[2][6];
        int config_cursor_pos_size;
        int config_cursor_state;
        int config_list;
        int config_value[5];

        void config_parser(int index);

        //Security
         int passcode[4]={0,0,0,0};
         int passcode_try[4]={0,0,0,0};
         int security_cursor_pos[2][5];
         int security_cursor_state=0;
         int security_cursor_pos_size;
         void reset_passcode_try();

        /********************************UTILITAIRE**********************************/
        String intToChar(int number, int nb_digit); 
        int min0max100(int number);
        int minMax(int value, int min_value, int max_value);
        
        //Config


   

    
};

#endif // LCD_MENU_H
