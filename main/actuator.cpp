//
// Created by xaviert on 2020-03-28.
#include <Arduino.h>
#include "actuator.h"

const int STEPS_PER_REV = 200;
Stepper stepper_NEMA17(STEPS_PER_REV, 22, 23, 24, 25);
const int STEPPER_MAX_SPEED = 200;
int stepperPosition = 0;

#ifdef PNEUMATIC

int finDeCourse = 36;
double dts[5];
int distributeur_pneu = 51; 
int valve_expi = 47; 
/* v
 * This function calibrate the piston's speed
 */
void Calibrate_motor(){
    pinMode(34, OUTPUT);
    pinMode(distributeur_pneu,OUTPUT); // 
    pinMode(valve_expi,OUTPUT); //
    pinMode(finDeCourse, INPUT_PULLUP);
    digitalWrite(34, HIGH);
    delay(1000);
    stepper_NEMA17.setSpeed(STEPPER_MAX_SPEED);
    for(int i = 0; i < 4; i++){
        stepper_NEMA17.step(-2 * STEPS_PER_REV);
        double temps0 = millis();
        digitalWrite(distributeur_pneu,HIGH);
        digitalWrite(valve_expi,LOW);
        Serial.print("Start course ");
        Serial.println(digitalRead(finDeCourse));
        bool enCourse = digitalRead(finDeCourse);
        while(enCourse){
          enCourse = digitalRead(finDeCourse);
          Serial.println(enCourse);
        }
          Serial.println("End course");
          double temps1 = millis();
          digitalWrite(distributeur_pneu,LOW);
          digitalWrite(valve_expi,HIGH);
          double dt = (temps1 - temps0)/1000;
          dts[i] = dt;
          delay(500);
          Serial.print("delta temps: ");
          Serial.println(dt);
        }
        digitalWrite(34, LOW);
    // fait 5 fois:
    // fait 2 tours
    // part un timer
    // sort le piston jusqu'à la limit switch
    // trouve dt
    // affiche dt
}
/*
 * This function modulate the inhale flow
 * input : inhale flow value ml/sec
 * output: modulation
 */
 
void speedControl(double tempsInspi){
}

/*
 * This function activate the actuator to push air
 */
/*
 * This function stop the airflow
 */
void actuatorStop(){
    actuatorExhale();
}

/*
 * This function activate the actuator in the opposite direction at full speed
 */

#endif //PNEUMATIC

#ifdef MECANIC
/*
 * This function modulate the inhale flow
 * input : inhale flow value ml/sec
 * output: modulation
 */
void speedControl(double flow){
    // calcul du rpm en fcn du flow demandé

    stepper_NEMA17.setSpeed(motorSpeed);

}

/*
 * This function activate the actuator to push air
 */
void actuatorInhale(){
    stepper_NEMA17.step(2);
}

/*
 * This function stop the airflow
 */
void actuatorStop(){
    return;
}

/*
 * This function activate the actuator in the opposite direction at full speed
 */
void actuatorExhale(){
    stepper_NEMA17.setSpeed(-STEPPER_MAX_SPEED);
    stepper_NEMA17.step(2);
}

#endif //MECANIC
