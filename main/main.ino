#include "Wire.h"
#include "pressure.h"
#include "lcd_menu.h"
#include <BME280I2C.h>

#define pi 3.1415926
#define Pa_2_cmH2O 0.0101972
#define cmH2O_2_Pa 98.0665

// * Different states of the machine * //
// Associated variable: current_state
#define INSPIRATION 1
#define EXPIRATION 2
#define STOP 3

// * ALARMS * //
// Associated variable: alarm
#define NO_ALARM 0
#define LOW_PRESSURE 1
#define HIGH_PRESSURE 2 
#define DISCONNECT 3
#define LOW_FiO2 4


// * Physical pins needed * //
#define buzzer_pin 2
#define FiO2_pin A6
#define valve_pin 22
#define limitswitch_pin 26

// * I2C BUS * //
#define SERIAL_BAUD 9600
#define psensor1 0x28 // Venturi flow sensor
#define psensor2 0x28 // Pressure sensor
#define psensor3 0x28 // Calibration sensor

// *** Doctor variables *** //
int resp_per_minute = 30; // cycles/minute
float ie_ratio = 0.3; // %
int hp_pressure_h20 = 21; // cmH2O
int tidal_volume = 410; // ml
int FiO2_target = 30;  // % 


// *** Measure *** //
float freq_cycle = resp_per_minute / 60.0;
float time_inspi = ie_ratio / freq_cycle;
float time_expi = (1-ie_ratio) / freq_cycle;
float atm = 100000;

// *** Variables *** //
int alarm = 0;
int index; 
int sound_freq[2] = {0,0} ;

float pressure = 0;
float pressure2 = 0;
float high_pressure = 10000;
float low_pressure = -5000;
float disco_dP = 500;
int current_state = INSPIRATION; // 1 = Inspiration; 2 = Expiration; 3 = Stopped;
int prev_state = INSPIRATION;
int FiO2 = 20;
float density = 1.225; // [kg/m^3]


int timer_init = 0;
int timer_current = 0;
int timer_prev = 0;

// Frequency of the code
float frequency = 60; // [hz]
float period = 1/frequency * 1000; // [ms]

Pressure_gauge venturi_sensor(psensor1);
Pressure_gauge pressure_sensor(psensor2);
Pressure_gauge pressure_sensor3(psensor3);

Lcd_menu lcd_menu; 

// *** Shared functions *** // 
// Module
int mod(int a, int b) {
  int c = a % b;
  // Inline if (cond) ? si vrai : si faux
  return (c < 0) ? c + b : c;}

// For mean average uses
float mean_float(float *arr, int SizeOfArray ){
  int test =0;
  for (int i=0; i<SizeOfArray;i++){
    test = test + arr[i]/SizeOfArray;  
  }
  return test;
}
int mean_int(int *arr, int SizeOfArray ){
  float test =0;
  for (int i=0; i<SizeOfArray;i++){
    test = test + arr[i]/SizeOfArray;  
  }
  return test;
}


// *** Bands *** //

// High priority functions
void band1_0() {
  state_machine(); // Cycle and time management
  FiO2_sense();
  check_FiO2();
  check_disconnect(); 
  check_HP();
  check_LP();
  //pressure_sensor_read2();
  //bme280_getdata();
  //valve_control();
  //venturi_measure();
  get_lcd();
  refresh_lcd();
}
// Mid priority functions part 1
void band2_0() {
  sound();

}
// Mid priority functions part 2
void band2_1() {
}
// Low priority functions part 1
void band3_0() {
}
// Low priority functions part 2
void band3_1() {
}
// Low priority functions part 3
void band3_2() {
}
// Low priority functions part 4
void band3_3() {
}

// *** Setup and loop *** //

void setup() {
  // Starting serial communication
  Serial.begin(SERIAL_BAUD);
  while(!Serial) {} // Wait
  Wire.begin();
  // Setting up the individual functionalities
  setup_lcd();
  //bme280_setup();
  pinMode(buzzer_pin, OUTPUT); // Set buzzer
  //venturi_sensor.calibrate();
  //pressure_sensor2.calibrate();
  pinMode(valve_pin,OUTPUT);
  // Initialisation Timer
  timer_init = millis();
  pinMode(13,OUTPUT);
  Serial.println(resp_per_minute);
}

void loop() {
   switch (mod(index,4)){
    case 0:
      band1_0();
      band2_0();
      band3_0();
      break;
    case 1:
      band1_0();
      band2_1();
      band3_2();
      break;
    case 2:
      band1_0();
      band2_0();
      band3_1();
      break;
    case 3:
      band1_0();
      band2_0();
      band3_1();
      break;
    }
    index++;
}

// *** Functions to be distributed inside the multiple bands *** //

// * State Machine * //
// Function variables // 
int timer_state = 0; 
int timer_state_prev = 0; 

// Function //
void state_machine(){
  
  timer_state = millis();
  if (alarm == DISCONNECT){
    current_state = STOP; 
    // IF BUTTON PRESS, INSPIRATION
  }
  // INSPIRATION --> EXPIRATION
  else if (current_state == INSPIRATION &&  (timer_state - timer_state_prev) >= time_inspi*1000 ){
    current_state = EXPIRATION;
    timer_state_prev = timer_state;
    }
  // EXPIRATION --> INSPIRATION
  else if(current_state == EXPIRATION && (timer_state - timer_state_prev) >= time_expi*1000){
    current_state = INSPIRATION;
    timer_state_prev = timer_state;
    }
  
   // Assisted ventilation mode switch (TBD)
   //else if (pressure < low_pressure && mode == assisted){
   //current_state = !current_state
   //timer_state_prev = millis();
   //}
   //else if (pressure > high_pressure && mode == assisted){
   //current_state = !current_state
   //timer_state_prev = millis();
   //}
}

// * Low-Pressure Alarm (1) * //
// Function Variables //
bool lp_state = false;
bool lp_state_prev = false;
float lp_timer = 0;
float lp_timer_prev = 0;
int lp_duration = 200;
int lp_tones[2] = {600, 300};
int lp_index = 0;

// Function //
void check_LP(){
  if (pressure < low_pressure && alarm == NO_ALARM )
  {
    alarm = LOW_PRESSURE;
     /* DATA to feed to the sound function */
    //sound_freq = lp_tones;
    memcpy(sound_freq, lp_tones, sizeof(sound_freq[0])*2);
  }
  else if (alarm == LOW_PRESSURE && pressure > low_pressure){
    alarm = NO_ALARM; 
  }
}

// * High-Pressure Alarm (2) * //
// Function Variables //
bool hp_state = false;
bool hp_state_prev = false;
float hp_timer = 0;
float hp_timer_prev = 0;
int hp_duration = 200;
int hp_tones[2] = {2000,1000};
int hp_index = 0;

// Function //
void check_HP(){
  if (pressure > high_pressure && alarm == NO_ALARM )
  {
    alarm = HIGH_PRESSURE;
     /* DATA to feed to the sound function */
    //sound_freq = hp_tones;
    memcpy(sound_freq, hp_tones, sizeof(sound_freq[0])*2);
  }
  else if (alarm == HIGH_PRESSURE && pressure < high_pressure){
    alarm = NO_ALARM; 
  }
}


// * Disconnect Alarm (3) * //
// Function Variables //
bool disconnect_state = false;
bool disconnect_state_prev = false;
float disconnect_timer = 0;
float disconnect_timer_prev = 0;
int disconnect_duration = 200;
int disconnect_tones[2] = {2000, 1000};
int disconnect_index = 0;

// Function //
void check_disconnect(){
  if (current_state == INSPIRATION){
    // patient pressure is at atmosphere && 10% of the inspiration time has passed and  
    if (abs(pressure) < disco_dP && (timer_state - timer_state_prev) >= 0.1 * time_inspi)
    {
      alarm = DISCONNECT;
      memcpy(sound_freq, disconnect_tones, sizeof(sound_freq[0])*2);
    }
    else if(abs(pressure) > disco_dP && (timer_state - timer_state_prev) >= 0.1 * time_inspi ){
      alarm = NO_ALARM;
    }
  }
}

// * Low-FiO2 alarm (4) * //
// Function Variables //
int FiO2_set = 20;
int FiO2_delta = 10;
int FiO2_tones[2] = {1200, 1000};
// Function //
void check_FiO2(){
  if (FiO2 < FiO2_set - FiO2_delta && alarm == NO_ALARM)
  {
    alarm = LOW_FiO2;
     /* DATA to feed to the sound function */
    //sound_freq = lp_tones;
    memcpy(sound_freq, FiO2_tones, sizeof(sound_freq[0])*2);
  }
  else if (alarm == LOW_FiO2 && FiO2 > FiO2_set - FiO2_delta){
    alarm = NO_ALARM; 
  }
}
// * SOUND TONES * //
// Variables //
int sound_index = 0;
// Function //
void sound(){
  if(alarm == NO_ALARM){
    noTone(buzzer_pin);
  }
  else{
    tone(buzzer_pin,sound_freq[sound_index]);
    sound_index +=1;
  }
}

// * Read O2 sensor * //
// Variables //
float FiO2_raw[3] = {20,20,20}; // Initialize at atmospehre % to evade alarms
int FiO2_index = 0;
float FiO2_percent[2] = {21,  100} ;
float FiO2_cal_array[2] ={126, 617}; // [0-1024 scale] calibration values for the FiO2 sensor (after op-amp) 
float FiO2_slope = 0;
float FiO2_const = 0;
// Functions //
// Calibration via the values inside the array //
void FiO2_cal(){
  FiO2_slope = (FiO2_cal_array[1] - FiO2_cal_array[0])/ (FiO2_percent[1]-FiO2_percent[1]);
  FiO2_const = FiO2_cal_array[0] - FiO2_slope*FiO2_percent[0];
}

// Detecting FiO2 //
void FiO2_sense(){
  // Make sure the O2 sensor is connected to an op-amplifier
  FiO2_raw[mod(FiO2_index,3)] = analogRead(FiO2_pin)*FiO2_slope + FiO2_const;
  FiO2 = (int) mean_float(&FiO2_raw[0],3);
  FiO2_index +=1;
}

// * Controlling the actuated valve * //
// Variables //
// No local variables needed
// 
void valve_control(){
  if (current_state == INSPIRATION ){
    digitalWrite(valve_pin,HIGH);
  }
  else{
    digitalWrite(valve_pin,LOW);
  }
}

// * Read Pressure sensor BME* //
// Variables //
BME280I2C bme;
float bme_pressure;
float bme_offset_pressure;
float bme_humidity;
float bme_temperature;
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);
// Functions //
void bme280_setup(){
  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  // bme.chipID(); // Deprecated. See chipModel().
  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }
    float sum=0;
    int nb = 10;
    for(int i=0;i<nb;i++)
    {
      bme.read(bme_pressure, bme_temperature, bme_humidity, tempUnit, presUnit);
      sum = sum+ bme_pressure; 
    }
    bme_offset_pressure = sum/nb;
}
void bme280_getdata(){
   bme.read(bme_pressure, bme_temperature, bme_humidity, tempUnit, presUnit);
   pressure = bme_pressure - bme_offset_pressure; // This is currently the main pressure sensor
}

// * Pressure sensor * //
// Variables //
int prev_state_pressure = current_state;
int measured_pressure_inspi = 0;
int measured_pressure_expi = 0;
float measured_pressure = 0;
int nb_data_inspi = 0;
// Function //
void pressure_sensor_read(){
    pressure_sensor.send();
    pressure_sensor.read();
    pressure = pressure_sensor.get_pressure();
}
void calc_display_pressure(){
  if (current_state == prev_state_pressure && current_state == INSPIRATION){ // DIsplay mean pressure inside the lung
      measured_pressure = max(measured_pressure, pressure);
  }
  if (current_state == prev_state_pressure && current_state == EXPIRATION){ // DIsplay mean pressure inside the lung
      measured_pressure = min(measured_pressure, pressure);
  }
  else if(current_state == EXPIRATION && prev_state_pressure == INSPIRATION){
     measured_pressure_inspi = (int) (Pa_2_cmH2O * measured_pressure);
     // Reset mean and counters
     measured_pressure = pressure;
  }
  else if(current_state == INSPIRATION && prev_state_pressure == EXPIRATION){
   measured_pressure_expi = (int) (Pa_2_cmH2O *  measured_pressure); // Convert Pa to cm H2O
   // Reset mean and counters
   measured_pressure = pressure;
  }
  prev_state_pressure = current_state;
}

// * LCD * //
// Variables //

int lcd_ie_ratio = 0;
int input_tidal_volume;
int measured_tidal_volume;

// Old entered values
int old_input_tidal_volume = input_tidal_volume;
int old_high_pressure = high_pressure;
int old_resp_per_minute = resp_per_minute;
float old_ie_ratio = ie_ratio;
int old_FiO2_set = FiO2_set;

// Old display variables
int lcd_alarm = alarm;
int lcd_current_state = current_state;
int lcd_measured_tidal_volume = measured_tidal_volume;
int lcd_measured_pressure_inspi = measured_pressure_inspi;
int lcd_measured_pressure_expi = measured_pressure_expi;
int lcd_FiO2 = FiO2;
int lcd_button;
// Function //
void setup_lcd(){
  lcd_menu.set_TidalVolume_cmd((int) (tidal_volume)); 
  lcd_menu.set_InspiPressure_cmd(hp_pressure_h20); // Pressure differential in cmH20
  lcd_menu.set_RespiratoryRate_cmd(resp_per_minute); 
  lcd_menu.set_IERatio_cmd((int) ie_ratio*10);
  lcd_menu.set_FiO2Target_cmd(FiO2_target);
  lcd_menu.set_passcode(0,0,0,0);
  lcd_menu.lcd_run();
 }
 void get_lcd(){
  // Inputs from the doctor
  lcd_button = lcd_menu.read_LCD_buttons();
  if (lcd_button != 5){ // Is a button read
      lcd_menu.lcd_run();
      lcd_menu.read_LCD_buttons();
      lcd_menu.lcd_run();
      input_tidal_volume = lcd_menu.get_TidalVolume_cmd(); //Objectif de tidal volume
      high_pressure = cmH2O_2_Pa * lcd_menu.get_InspiPressure_cmd(); // Peak pressure
      resp_per_minute = lcd_menu.get_RespiratoryRate_cmd();
      ie_ratio = ((float) lcd_menu.get_IERatio_cmd())/10;
      FiO2_set = lcd_menu.get_FiO2Target_cmd();
     
   }
}
 void refresh_lcd(){
  if (alarm != lcd_alarm || current_state != lcd_current_state || 
    measured_tidal_volume != lcd_measured_tidal_volume || measured_pressure_inspi != lcd_measured_pressure_inspi ||
    measured_pressure_expi != lcd_measured_pressure_expi || FiO2 != lcd_FiO2 ){
    
     lcd_menu.startAlarm((int) alarm);
     lcd_menu.setStateMachine(current_state);
     lcd_menu.set_TidalVolume_reading(measured_tidal_volume);
     lcd_menu.set_Inspi_pressure(measured_pressure_inspi);
     lcd_menu.set_Peep_pressure(measured_pressure_expi);
     lcd_menu.set_FiO2(FiO2);
    
     // Values currently inside the lcd
     lcd_alarm = alarm;
     lcd_current_state = current_state;
     lcd_measured_tidal_volume = measured_tidal_volume;
     lcd_measured_pressure_inspi = measured_pressure_inspi;
     lcd_measured_pressure_expi = measured_pressure_expi;
     lcd_FiO2 = FiO2;
     lcd_menu.lcd_run();
   }   
 }
/*
// * Venturi Flowmeter * //
// Variables //
float venturi_speed = 0;
float venturi_pressure = 0;
float venturi_flow = 0;
float venturi_small_radi = 0.00635; 
float venturi_normal_radi = 0.0127;

float venturi_volume = 0;
float venturi_volume_prev = 0;
float venturi_time = 0;
float venturi_time_prev = 0;
int venturi_prev_state =0;

float venturi_small_section = pi*pow(venturi_small_radi,2); 
float venturi_normal_section = pi*pow(venturi_normal_radi,2); 

// Setup // 
// Calculate // 
void venturi_measure(){
  venturi_sensor.send();
  venturi_sensor.read();
  venturi_pressure = venturi_sensor.get_pressure();
  dummy = abs(2*venturi_pressure/(density*(1-pow(venturi_normal_section,2)/pow(venturi_small_section,2))));
  venturi_speed = sqrt(dummy);
  venturi_flow = venturi_speed * pow(venturi_normal_radi,2) * pi;
  //Serial.print("venturi: ");
  //Serial.println(venturi_pressure);
  //Serial.print("venturi_flow: ");
  //Serial.println(dummy);
  Serial.println(venturi_speed);
 }
void venturi_TidalVolume(){
  venturi_time = millis();
  if (current_state == INSPIRATION){
    venturi_volume = venturi_volume + venturi_flow*(venturi_time-venturi_time_prev)/1000;
  }
  else if (current_state != venturi_prev_state){
    venturi_volume = venturi_volume_prev;
    venturi_volume=0;
  }
  current_state = venturi_prev_state;
  venturi_time_prev = venturi_time;
}
*/
