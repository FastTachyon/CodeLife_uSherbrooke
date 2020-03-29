#ifndef LCD_MENU_H
#define LCD_MENU_H
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>
#include "screen.h"
#include <EEPROM.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define InspiPressure 0
#define TidalVolume 1
#define RespiratoryRate 2
#define IERatio 3
#define FiO2Target 4

#define NB_CONFIG 5

class Lcd_menu
{
    public:
        //Constructeur, initilise beaucoup de variables
        Lcd_menu();

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
        
        int read_LCD_buttons();
    protected:

    private:
        //LCD 
        LiquidCrystal lcd;


        //Boutons
        
        int lcd_key;
        int adc_key_in;
        int button_pressed=5;
        
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
        Screen security;
        Screen settings;
        Screen display;

        //config
        int config_list;
        int config_value[NB_CONFIG];
        Config config[NB_CONFIG];

        void config_parser(int index);

        //Security
         int passcode[4]={0,0,0,0};
         int passcode_try[4]={0,0,0,0};
         void reset_passcode_try();

        void writeEEPROM();
        void readEEPROM();

        /********************************UTILITAIRE**********************************/
        String intToChar(int number, int nb_digit); 
        int min0max100(int number);
        int minMax(int value, int min_value, int max_value);
        
     

    
};

#endif // LCD_MENU_H
