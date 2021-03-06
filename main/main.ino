#include "Wire.h"
#include "pressure.h"
#include "lcd_menu.h"
#include <BME280I2C.h>
#include "actuator.h"

#define pi 3.1415926
#define Pa_2_cmH2O 0.0101972
#define cmH2O_2_Pa 98.0665

// * Different states of the machine * //
// Associated variable: current_state
#define INSPIRATION 1
#define EXPIRATION 2
#define STOP 3
#define CALIBRATION 4

// * ALARMS * //
// Associated variable: alarm
#define NO_ALARM 0
#define LOW_PRESSURE 1
#define HIGH_PRESSURE 2 
#define DISCONNECT 3
#define LOW_FiO2 4


// * Physical pins needed * //
#define buzzer_pin 43
#define FiO2_pin A7
#define limitswitch_pin 26
#define valve_pneu 51
#define valve_expi 47

// * I2C BUS * //
#define SERIAL_BAUD 115200
#define psensor 0x28 // Venturi flow sensor

// ** Calibration ** //
float FiO2_percent[2] = {21,  100} ;
float FiO2_cal_array[2] ={126, 617}; // [0-1024 scale] calibration values for the FiO2 sensor (after op-amp) 

// *** Input Variables (from LCD)*** //
int resp_per_minute = 30; // cycles/minute
float ie_ratio = 0.3; // %
int hp_pressure = 2059.4; // Alarm condition
int tidal_volume = 410; // ml
int FiO2_target = 30;  // Alarm condition 

// *** Get initial times *** //
float freq_cycle = resp_per_minute / 60.0; // Frequence de respiration
float time_expi = ie_ratio / freq_cycle; // Calculating the initial expiration time
float time_inspi = (1-ie_ratio) / freq_cycle; // Calculating the initial inspiration time

// *** Variables *** //
int alarm = 0; // Shared variable; Current alarm state of the system
int index; // Modulo index to switch between the different states
int sound_freq[2] = {0,0} ; // Sound function array

float pressure = 0; // Shared variable for pressure (alarm, LCD, motor control)
float high_pressure = 10000; // Condition for high pressure alarm (alarm, LCD)
float low_pressure = -5000; // Condition for low pressure (alarm, LCD, assisted resp (NOT YET READY))
float disco_dP = 500; // Condition for disconnection detect (alarm)
int current_state = INSPIRATION; // 1 = Inspiration; 2 = Expiration; 3 = Stopped; (state machine)
int FiO2 = 20; //
float density = 1.225; // [kg/m^3]
int measured_pressure_inspi = 0; // Maximum pressure measured during inspiration
int measured_pressure_expi = 0; // Minimum pressure measured during inspiration
int measured_tidal_volume = 0; // Measured tidal pressure taken from venturi tube

Pressure_gauge venturi_sensor(psensor); // Initialising venturi sensor

BME280I2C bme; // Initialising BME280 pressure sensor

Lcd_menu lcd_menu;  // Initialising the LCD menu. 

// *** Shared functions *** // 

// Modulo for index gestion of arrays
int mod(int a, int b) {
  int c = a % b;
  // Inline if (cond) ? si vrai : si faux
  return (c < 0) ? c + b : c;}

// For mean average purposes
float mean_float(float *arr, int SizeOfArray ){
  float test =0;
  for (int i=0; i<SizeOfArray;i++){
    test = test + arr[i]/SizeOfArray;  
  }
  return test;
}
int mean_int(int *arr, int SizeOfArray ){
  int test =0;
  for (int i=0; i<SizeOfArray;i++){
    test = test + arr[i]/SizeOfArray;  
  }
  return test;
}


// *** Bands *** //

// High priority functions
void band1_0() {
  state_machine(); // Cycle and time management
  FiO2_sense(); // Sensing FiO2 of the air inside the system
  check_FiO2(); // Alarm if the FiO2 is over the alarm or not
  check_disconnect(); // Alarm for a disconnection of the system
  check_HP(); // Alarm for high pressure
  check_LP(); // Alarm for low pressure
  //pressure_sensor_read2();
  bme280_getdata(); // Read the BME280 sensor
  venturi_measure(); // Read the differential sensor
  #ifdef PNEUMATIC
  valve_pneumatic(); // Activate pneumatic valve
  #elif def MECANIC
  stepper_control();
  #endif
  valve_expiration(); // Activate actuated valve
  //Serial.println(current_state);
}
// Mid priority functions part 1
void band2_0() {
  get_lcd(); // Get data fed by the LCD
}
// Mid priority functions part 2
void band2_1() {
  sound(); // Trigger the tones made by the sensor
  venturi_TidalVolume(); // Calculate the Tidal volume from the delta P
}
// Low priority functions part 1
void band3_0() {
  refresh_lcd(); // Update LCD readout with the newest measurements
}
// Low priority functions part 2
void band3_1() {
  calc_cycle_pressure(); // Calculate the display pressure 
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
  while(!Serial) {} // Wait for the communication to initialise
  
  Wire.begin();  // Initialising wire library for I2C communication
  
  Serial.println("Calibration started");
  // Setting up the individual functionalities
#ifdef PNEUMATIC
  Calibrate_pneu(); // Motor calibration
#elif def Mecanic
  //Calibrate_meca();
#endif
  setup_lcd(); // Setting up the LCD with the right initialisation values
  bme280_setup(); // Calibrating the BME280  
  setup_sound(); // Setting up the sound 
  venturi_sensor.calibrate(); // Calibrating the venturi (setting up the first offset)
  FiO2_cal(); // Calibrating the FiO2 sensor using the calibration array values
  Serial.println("Calibration ended");
  // Initialisation Timer
  Serial.println("Ventilation started");
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
// Timed function with high priority which define the current state of the system.
// Function variables // 
int timer_state = 0; 
int timer_state_prev = 0; 

// Function //
void state_machine(){
  timer_state = millis();
  // INSPIRATION --> EXPIRATION
  if (current_state == INSPIRATION &&  (timer_state - timer_state_prev) >= time_inspi*1000 ){
    current_state = EXPIRATION;
    timer_state_prev = timer_state;
    }
  // EXPIRATION --> INSPIRATION
  else if(current_state == EXPIRATION && (timer_state - timer_state_prev) >= time_expi*1000){
    current_state = INSPIRATION;
    timer_state_prev = timer_state;
    }
  
   // Assisted ventilation mode switch (TBD)
  // else if (pressure < low_pressure && current_state == EXPIRATION){
  //    current_state = INSPIRATION;
  //    timer_state_prev = timer_state;
  // }
   //else if (pressure > high_pressure && mode == assisted){
   //current_state = !current_state
   //timer_state_prev = millis();
   //}
}

// * Low-Pressure Alarm (1) * //
// Check for low pressure alarm
// Function Variables //
int lp_tones[2] = {600, 300};

// Function //
void check_LP(){
  if (measured_pressure_expi < low_pressure && alarm == NO_ALARM )
  {
    alarm = LOW_PRESSURE;
     /* DATA to feed to the sound function */
    //sound_freq = lp_tones;
    memcpy(sound_freq, lp_tones, sizeof(sound_freq[0])*2);
  }
  else if (alarm == LOW_PRESSURE && measured_pressure_expi > low_pressure){
    alarm = NO_ALARM; 
  }
}

// * High-Pressure Alarm (2) * //
// Check for high pressure alarm
// Function Variables //
int hp_tones[2] = {2000,1000};

// Function //
void check_HP(){
  if (measured_pressure_inspi > high_pressure && alarm == NO_ALARM )
  {
    alarm = HIGH_PRESSURE;
     /* DATA to feed to the sound function */
    //sound_freq = hp_tones;
    memcpy(sound_freq, hp_tones, sizeof(sound_freq[0])*2);
  }
  else if (alarm == HIGH_PRESSURE && measured_pressure_inspi < high_pressure){
    alarm = NO_ALARM; 
  }
}


// * Disconnect Alarm (3) * //
// Condition to check for a system disconnect.
// Only verified during inspiration while bag is being contracted since we use the pressure.
// Function Variables //
int disconnect_tones[2] = {2000, 1000};

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
void setup_sound(){
  pinMode(buzzer_pin, OUTPUT); // Set buzzer
  Serial.println("Buzzer pin is Ready");
}
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
//Reading the analog pin associated with the O2 sensor. 
// Variables //
float FiO2_raw[3] = {20,20,20}; // Initialize at atmospehre % to evade alarms
int FiO2_index = 0;
float FiO2_slope = 0;
float FiO2_const = 0;
// Functions //
// Calibration via the values inside the array //
void FiO2_cal(){
  Serial.print("Using calibration array values for FiO2 data setup");
  FiO2_slope = (FiO2_percent[1]-FiO2_percent[0]) / (FiO2_cal_array[1] - FiO2_cal_array[0]) ;
  FiO2_const = FiO2_percent[0] - FiO2_slope*FiO2_cal_array[0];
}

// Detecting FiO2 //
void FiO2_sense(){
  // Make sure the O2 sensor is connected to an operational-amplifier
  FiO2_raw[mod(FiO2_index,3)] = analogRead(FiO2_pin)*FiO2_slope + FiO2_const;
  FiO2 = (int) mean_float(&FiO2_raw[0],3);
  FiO2_index +=1;
}

// * Controlling the actuated valve * //
// Variables //
// No local variables needed
// Function //
void valve_expiration(){
  if (current_state == INSPIRATION ){
	// During inspiration, stay closed
    digitalWrite(valve_expi,HIGH);
  }
  else{
	// During expiration, open
    digitalWrite(valve_expi,LOW);
  }
}

#ifdef PNEUMATIC
// * Controlling the pneumatic valves * //
// Wrapper for actuator library
// Variables //
// No local variables needed
// Function //
void valve_pneumatic(){
	speedControl(time_inspi); // Consctricting flow more or less
  if (current_state == INSPIRATION && (pressure < high_pressure) ){
    digitalWrite(valve_pneu,LOW);
  }
  else if(current_state = EXPIRATION ){
    digitalWrite(valve_pneu,HIGH);
  }
  else{
      digitalWrite(valve_pneu,HIGH);
    }
}  
#endif
#ifdef MECANIC
// * Controlling the pneumatic valves * //
// Not tested with the electric option yet. TO BE MODIFIED
// Variables //
// No local variables needed
void stepper_control(){
  speedControl(time_inspi); // Consctricting flow more or less
  if (current_state == INSPIRATION && (pressure < high_pressure) ){
    digitalWrite(valve_pneu,LOW);
  }
  else if(current_state = EXPIRATION ){
    digitalWrite(valve_pneu,HIGH);
  }
  else{
      digitalWrite(valve_pneu,HIGH);
    }
}
#endif

// * Read Pressure sensor BME* //
// Multifunction pressure sensor. Measures pressure, temperature and humidity with a relative precision. 
// The sensor is placed on the expiratory circuit, before the actuated valve but after the filter. 
// The pressure values are used to measure patient lung pressure.
// The temperature values are used for monitoring the patient (TO BE IMPLEMENTED)
// The humidity values are used to regulate the humidifier (TO BE IMPLEMENTED)  
// Note: This function can be replaced with the wrapper for the main pressure sensor used in the setup
// Variables //
float bme_pressure;
float bme_offset_pressure;
float bme_humidity;
float bme_temperature;
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);
// Functions //
// Detecting the sensor. This is a stop point as this sensor is NECESSARY for safe operation fo the system.
void bme280_setup(){
  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!"); 
    delay(1000);
  }
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
  // Measuring a mean offset for the pressure of the system. 
    float sum=0;
    int nb = 10;
    for(int i=0;i<nb;i++)
    {
      bme.read(bme_pressure, bme_temperature, bme_humidity, tempUnit, presUnit);
      sum = sum+ bme_pressure; 
    }
    bme_offset_pressure = sum/nb;
}
// Getting data from the BME280 sensor
void bme280_getdata(){
   bme.read(bme_pressure, bme_temperature, bme_humidity, tempUnit, presUnit);
   pressure = bme_pressure - bme_offset_pressure; // This is currently the main pressure sensor
}
/* Commented because this piece of code is NOT FINISHED. 
Interfacing is possible with multiple pressure sensors which do not have the same I2C adress. 
The adress of the pressure sensors are defined at the header of the code.
// * Pressure sensor * //
// Variables //
//  Setup  //
void pressure_sensor_setup(){
  Serial.println("Starting pressure offset calibration");
  pressure_sensor.calibrate();
  Serial.println("Finished calibrating pressure sensor");
  }
// Function //
void pressure_sensor_read(){
    pressure_sensor.send();
    pressure_sensor.read();
    pressure = pressure_sensor.get_pressure();
    //Serial.println(pressure);
}
*/
// * Pressure values read by alarms and displays * //
// Function variables //
int prev_state_pressure = current_state;
float measured_pressure = 0;
// Function //
// Calculating the maximal the pressure to display and use for alarms using global variables
void calc_cycle_pressure(){
  if ((current_state == prev_state_pressure) && (current_state == INSPIRATION)){ // DIsplay mean pressure inside the lung
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
// The following functions are made using LCD
// Only refresh the display if the variables are changed inside the code
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
  lcd_menu.set_InspiPressure_cmd(hp_pressure * Pa_2_cmH2O); // Pressure differential in cmH20
  lcd_menu.set_RespiratoryRate_cmd(resp_per_minute); 
  lcd_menu.set_IERatio_cmd((int) ie_ratio*10);
  lcd_menu.set_FiO2Target_cmd(FiO2_target);
  lcd_menu.set_passcode(0,0,0,0);
  lcd_menu.lcd_run();
 }
 void get_lcd(){
  // get inputs from the doctor and update inside the code
  lcd_button = lcd_menu.read_LCD_buttons();
  if (lcd_button != 5){ // Is a button read
      lcd_menu.lcd_run();
      lcd_menu.read_LCD_buttons();
      lcd_menu.lcd_run();
      tidal_volume = lcd_menu.get_TidalVolume_cmd()*10; //Objectif de tidal volume (ml)
      high_pressure = cmH2O_2_Pa * lcd_menu.get_InspiPressure_cmd(); // Peak pressure
      resp_per_minute = lcd_menu.get_RespiratoryRate_cmd();
      ie_ratio = 1/(1+lcd_menu.get_IERatio_cmd());
      FiO2_target = lcd_menu.get_FiO2Target_cmd();

      freq_cycle = resp_per_minute / 60.0;
      time_expi = ie_ratio / freq_cycle;
      time_inspi = (1-ie_ratio) / freq_cycle;
   }
}
// Reresh the LCD with new  values to display. 
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

// * Venturi Flowmeter * //
// Measuring the flow using a venturi nozzle with a differential pressure sensor. 
// If the flow is over 5 m/s th measurment is precise. 
// Part number used by the setup is: SSCDRRN005PD2A5
// We recommend using a setup with a more precise sensor to reduce the uncertainty of the measurement.
// Variables //
float venturi_speed = 0;
float venturi_pressure[5] = {0,0,0};
float venturi_flow = 0;
float venturi_small_radi = 0.00535; 
//float venturi_normal_radi = 0.0127;
float venturi_normal_radi = 0.0127;
int venturi_index = 0;
float mean_venturi_data = 0;

float venturi_volume = 0;
float venturi_volume_prev = 0;
float venturi_time = 0;
float venturi_time_prev = 0;
int venturi_prev_state =0;

float venturi_small_section = pi*pow(venturi_small_radi,2); 
float venturi_normal_section = pi*pow(venturi_normal_radi,2); 

// Setup // 
// Taking an offset value and updating it.
void setup_venturi(){
  Serial.print("Starting venturi offset calibration");
  venturi_sensor.calibrate();
  Serial.print("Starting venturi offset calibration");
  }
// Calculating the speed and flow with given data // 
void venturi_measure(){
  venturi_sensor.send();
  venturi_sensor.read();
  venturi_pressure[mod(venturi_index,5)] = venturi_sensor.get_pressure();
  mean_venturi_data = mean_float(&venturi_pressure[0],5);
  // Removing noisy background for almost 0 speed (if flow is under 5 m/s)
  if (abs(mean_venturi_data) < 20.0){ // If above noise volume
    mean_venturi_data = 0;
  } 
  //Derivating speed from the pressure sensor (theorical approach)
  venturi_speed = sqrt(abs(2*mean_venturi_data/(density*(1-pow(venturi_normal_section,2)/pow(venturi_small_section,2)))));
  venturi_flow = venturi_speed * venturi_normal_section;
  venturi_index++;
  Serial.println(mean_venturi_data);
 }

// Integrating the flow by the time taken for an inspiration //
void venturi_TidalVolume(){
  venturi_time = millis();
  if (current_state == EXPIRATION){
    venturi_volume = venturi_volume + venturi_flow*(venturi_time-venturi_time_prev)/1000;
  }
  else if (current_state != venturi_prev_state){
    venturi_volume_prev = venturi_volume;
    measured_tidal_volume = venturi_volume;
    venturi_volume=0;
  }
  venturi_prev_state = current_state;
  venturi_time_prev = venturi_time;
}
