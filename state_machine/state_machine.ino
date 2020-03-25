#define NB_MA_PRESSURE 3

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


// * Physical pins needed * //

#define buzzer_pin 9
#define FiO2_pin A1


// *** Doctor variables *** //
int resp_per_minute;
int ie_ratio;
int hp_pressure;
int lp_pressure;
int tidal_volume;

// *** Measure *** //
float time_cycle = resp_per_minute / 60.0;
float time_inspi = ie_ratio * time_cycle;
float time_expi = (1-ie_ratio) * time_cycle;
float atm = 100000;

// *** Variables *** //
int alarm;
int index; 
int sound_freq[2] = {0,0} ;

float pressure = 0;
float high_pressure = 103000;
float low_pressure = 98000;
float disco_dP = 500;
int current_state = 3; // 1 = Inspiration; 2 = Expiration; 3 = Stopped;

int timer_init = 0;
int timer_current = 0;
int timer_prev = 0;

float frequency = 60;
float period = 1/frequency * 1000;

// *** Shared functions *** // 
// Module
int mod(int a, int b) {
  int c = a % b;
  // Inline if (cond) ? si vrai : si faux
  return (c < 0) ? c + b : c;}

// *** Bands *** //

// High priority functions
void band1_0() {
  state_machine();
  check_disconnect();
  check_HP();
  check_LP();
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
  pinMode(buzzer_pin, OUTPUT); // Set buzzer
  timer_init = millis();
}

void loop() {
  timer_current = millis();
 if (timer_current - timer_prev > period){
   switch (mod(index,4)){
    case 1:
      band1_0();
      band2_0();
      band3_0();
      break;
    case 2:
      band1_0();
      band2_1();
      band3_2();
      break;
    case 3:
      band1_0();
      band2_0();
      band3_1();
      break;
    case 4:
      band1_0();
      band2_0();
      band3_1();
      break;
    }
    index ++;
    timer_prev = timer_current;
 }
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
  else if (current_state == INSPIRATION &&  (timer_state - timer_state_prev) >= time_inspi ){
    current_state = EXPIRATION;
    timer_state_prev = timer_state;
    }
  // EXPIRATION --> INSPIRATION
  else if(current_state == EXPIRATION && (timer_state - timer_state_prev) >= time_expi){
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

// * Low-Pressure Alarm * //
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

// * High-Pressure Alarm * //
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


// * Disconnect Alarm * //
// Function Variables //
bool disconnect_state = false;
bool disconnect_state_prev = false;
float disconnect_timer = 0;
float disconnect_timer_prev = 0;
int disconnect_duration = 200;
int disconnect_tones[2] = {2000, 1000};
int disconnect_index = 0;

float MA_pressure[NB_MA_PRESSURE];
float MA_pressure_index = 0;
float current_MA_pressure = 0;

// Function //
void check_disconnect(){
  if (current_state == INSPIRATION){
    // patient pressure is at atmosphere && 10% of the inspiration time has passed and  
    if (abs(pressure - atm) < disco_dP && (timer_state - timer_state_prev) >= 0.1 * time_inspi)
    {
      alarm = DISCONNECT;
      memcpy(sound_freq, disconnect_tones, sizeof(sound_freq[0])*2);
    }
    else if(abs(pressure - atm) > disco_dP && (timer_state - timer_state_prev) >= 0.1 * time_inspi ){
      alarm = NO_ALARM;
    }
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
    tone(buzzer_pin,sound_freq[sound_index,2]);
    sound_index +=1;
  }
}

// * Read O2 sensor * //
// Variables //
float FiO2 = 0;
float FiO2_percent = {0.2,  0.4} 
float FiO2_cal_array ={126, 300}; // [0-1024 scale] calibration values for the FiO2 sensor (after op-amp) 
float FiO2_slope = 0;
float FiO2_const = 0;
// Functions //
// Calibration via the values inside the array //
void FiO2_cal(){
  FiO2_slope = (FiO2_cal_array[1] - FiO2_cal_array[0])/ (FiO2_percent[1]-FiO2_percent[1]);
  FiO2_const = FiO2_cal_array[0] - FiO2_slope[0]*FiO2_percent[0];
}

// Detecting FiO2 //
void FiO2_sense(){
  // Make sure the O2 sensor is connected to an op-amplifier
  FiO2 = analogRead(FiO2_pin)*FiO2_slope + FiO2_const;
}


// * Read Pressure sensor * //
// Variables //

// Functions //
