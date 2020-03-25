#include <BME280I2C.h>
#include <Wire.h>

#define SERIAL_BAUD 115200

#define NB_MA_PRESSURE 20

BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
                  // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

//////////////////////////////////////////////////////////////////

const int buzzer_pin = 9; //buzzer to arduino pin 9
const int duration = 200;
float pressure = 100000;
float disco_dP = 500;
float high_pressure = 103000;
float low_pressure = 98000;
float atm = 0; // Atmosphere pressure 
float MA_pressure[NB_MA_PRESSURE];
float MA_pressure_index = 0;
float current_MA_pressure = 0;
/* Modulo */ 

int mod(int a, int b) {
  int c = a % b;
  // Inline if (cond) ? si vrai : si faux
  return (c < 0) ? c + b : c;}

/* Moving average */
float MA(){
  float MA_pressure_value = 0;
  for (int i = 0 ; i < NB_MA_PRESSURE; i++){
     MA_pressure_value += MA_pressure[i]/NB_MA_PRESSURE;
  }
  return MA_pressure_value;
}


/* Fuite / Disconnection */
bool disconnect_state = false;
bool disconnect_state_prev = false;
float disconnect_timer = 0;
float disconnect_timer_prev = 0;
int disconnect_duration = 200;
int disconnect_tones[2] = {2000, 1000};
int disconnect_index = 0;

void check_disconnect(){
  current_MA_pressure = MA();
  if ((current_MA_pressure - atm) < disco_dP )
  {
    disconnect_state = true;
     /* Initiallisation de la déconnection */
     if (disconnect_state ){
      disconnect_timer = millis();
      disconnect_timer_prev = disconnect_timer - disconnect_duration - 1; // Make sure it starts as soon as it is caught
     }
     // Gérer la déconnection
     if (disconnect_state && (disconnect_timer - disconnect_timer_prev) > disconnect_duration ){
      tone(buzzer_pin, disconnect_tones[mod(disconnect_index,2)]);
      disconnect_index += 1;
      
      disconnect_timer_prev = disconnect_timer;
      disconnect_timer = millis();
     }
  }
  else{
    noTone(buzzer_pin);
    disconnect_state = false;
    
  }
}

/* High pressure check */

bool hp_state = false;
bool hp_state_prev = false;
float hp_timer = 0;
float hp_timer_prev = 0;
int hp_duration = 200;
int hp_tones[2] = {2000,1000};
int hp_index = 0;

void check_HP(){
  if (pressure > high_pressure )
  {
    hp_state = true;
     /* Initiallisation de la déconnection */
     if (hp_state ){
      hp_timer = millis();
      hp_timer_prev = hp_timer - hp_duration - 1; // Make sure it starts as soon as it is caught
     }
     // Gérer la déconnection
     if (hp_state && (hp_timer - hp_timer_prev) > hp_duration ){
      tone(buzzer_pin, hp_tones[mod(hp_index,2)]);
      hp_index += 1;
      
      hp_timer_prev = hp_timer;
      hp_timer = millis();
     }
  }
  else{
    noTone(buzzer_pin);
    hp_state = false;
    
  }
}


/* Low pressure check */

bool lp_state = false;
bool lp_state_prev = false;
float lp_timer = 0;
float lp_timer_prev = 0;
int lp_duration = 200;
int lp_tones[2] = {600, 300};
int lp_index = 0;

void check_LP(){
  if (pressure < low_pressure )
  {
    lp_state = true;
     /* Initiallisation de la déconnection */
     if (lp_state ){
      lp_timer = millis();
      lp_timer_prev = lp_timer - lp_duration - 1; // Make sure it starts as soon as it is caught
     }
     // Gérer la déconnection
     if (lp_state && (lp_timer - lp_timer_prev) > lp_duration ){
      tone(buzzer_pin, lp_tones[mod(lp_index,2)]);
      lp_index += 1;
      
      lp_timer_prev = hp_timer;
      lp_timer = millis();
     }
  }
  else{
    noTone(buzzer_pin);
    lp_state = false;
    
  }
}

float printBME280Data
(
   Stream* client
)
{
   float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);

   client->print("Temp: ");
   client->print(temp);
   client->print("°"+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
   client->print("\t\tHumidity: ");
   client->print(hum);
   client->print("% RH");
   client->print("\t\tPressure: ");
   client->print(pres);
   client->println(" Pa");

   return pres;
}

void setup(){
  pinMode(buzzer_pin, OUTPUT); // Set buzzer - pin 9 as an output

  Serial.begin(SERIAL_BAUD);

  while(!Serial) {} // Wait

  Wire.begin();

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

  pressure = printBME280Data(&Serial);
  atm = pressure;
}

void loop(){
check_disconnect();
check_HP();
check_LP(); 
pressure = printBME280Data(&Serial);
MA_pressure[mod(MA_pressure_index,NB_MA_PRESSURE)] = pressure;
}
