//
// Created by xaviert on 2020-03-28.
#include <Arduino.h>
#include "actuator.h"

const int STEPS_PER_REV = 200;                          // number of steps per revolution for the nema 17 stepper
Stepper stepper_NEMA17(STEPS_PER_REV, 22, 23, 24, 25);  // stepper definition
const int STEPPER_MAX_SPEED = 200;                      // max rpm for the nema 17
int stepperPosition = 0;                                // init stepper position

/*
 * This function activate the driver for the stepper motor
 */
void enableStepper(){
  digitalWrite(34, LOW);  
}

/*
 * This function disable the driver for the stepper motor
 */
void disableStepper(){
  digitalWrite(34, HIGH);  
}

#ifdef PNEUMATIC

int finDeCourse = 36;   // limit switch PIN
double dts[10];         // delta time list for calibration
int distributeur_pneu = 51;
int valve_expi = 50; 

/*
 * This function calibrate the piston's speed
 */
void Calibrate_pneu() {
    pinMode(34, OUTPUT);
    pinMode(distributeur_pneu, OUTPUT); //
    pinMode(valve_expi, OUTPUT); //
    pinMode(finDeCourse, INPUT_PULLUP);
    enableStepper();
    stepper_NEMA17.setSpeed(STEPPER_MAX_SPEED);
    digitalWrite(distributeur_pneu, LOW);
    digitalWrite(valve_expi, HIGH);
    delay(1000);
    for (int i = 0; i < 10; i++) {
        stepper_NEMA17.step(-1 * STEPS_PER_REV);
        double temps0 = millis();
        digitalWrite(distributeur_pneu, HIGH);
        digitalWrite(valve_expi, LOW);
        Serial.print("Start course ");
        Serial.println(digitalRead(finDeCourse));
        bool enCourse = digitalRead(finDeCourse);
        while (enCourse) {
            enCourse = digitalRead(finDeCourse);
            delay(10);
        }
        Serial.println("End course");
        double temps1 = millis();
        digitalWrite(distributeur_pneu, LOW);
        digitalWrite(valve_expi, HIGH);
        double dt = (temps1 - temps0) / 1000;
        dts[i] = dt;
        delay(500);
        Serial.print("delta temps: ");
        Serial.println(dt);
    }
    disableStepper();
    stepperPosition = 10 * STEPS_PER_REV;
}

/*
 * This function modulate the piston speed during inhale
 * input : inspiration time
 * output: modulation
 */
 
void speedControl(double tempsInspi){
    tempsInspi *= 0.9;
    double y1;
    double y2;
    int x1;
    int x2;
    if(tempsInspi > dts[1]){
        y1 = dts[0];
        y2 = dts[1];
        x1 = 1;
        x2 = 2;
    } else if(tempsInspi > dts[2]){
        y1 = dts[1];
        y2 = dts[2];
        x1 = 2;
        x2 = 3;
    } else if(tempsInspi > dts[3]){
        y1 = dts[2];
        y2 = dts[3];
        x1 = 3;
        x2 = 4;
    } else if(tempsInspi > dts[4]){
        y1 = dts[3];
        y2 = dts[4];
        x1 = 4;
        x2 = 5;
    } else if(tempsInspi > dts[5]){
        y1 = dts[4];
        y2 = dts[5];
        x1 = 5;
        x2 = 6;
    } else if(tempsInspi > dts[6]){
        y1 = dts[5];
        y2 = dts[6];
        x1 = 6;
        x2 = 7;
    } else if(tempsInspi > dts[7]){
        y1 = dts[6];
        y2 = dts[7];
        x1 = 7;
        x2 = 8;
    } else if(tempsInspi > dts[8]){
        y1 = dts[7];
        y2 = dts[8];
        x1 = 8;
        x2 = 9;
    } else{
        y1 = dts[8];
        y2 = dts[9];
        x1 = 9;
        x2 = 10;
    }
    double a = (y2 -y1) / (x2 - x1);
    double b = y1 - a * x1;
    double nbTours = (tempsInspi - b) / a;
    if (nbTours < 0){
        nbTours = 0.1;
    }
    int positionTarget = int(nbTours * STEPS_PER_REV);
    enableStepper();
    stepper_NEMA17.step(stepperPosition - positionTarget);
    stepperPosition = positionTarget;
    disableStepper();
}

/*
 * This function stop the airflow
 */
void actuatorStop(){
    actuatorExhale();
}

#endif //PNEUMATIC

#ifdef MECANIC

/*
 * This function modulate the stepper speed during inhale
 * input : inspiration time
 * output: modulation
 */

void speedControl(double tempsInspi){
}


/*
 * This function stop the airflow
 */
void actuatorStop(){
    return;
}



#endif //MECANIC
