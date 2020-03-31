//
// Created by xaviert on 2020-03-28.
//

#include "actuator.h"

#ifdef PNEUMATIC
uint8_t finDeCourse = 22;
pinMode(finDeCourse, INPUT_PULLUP);
double dts[5];
/*
 * This function calibrate the piston's speed
 */
void Calibrate(){
    stepper_NEMA17.setspeed(STEPPER_MAX_SPEED);
    for(int i = 0; i < 5; i++){
        double temps0 = millis();
        actuatorInhale();
        while(digitalRead(finDeCourse));
        double temps1 = millis();
        actuatorExhale();
        dt = (temps1 - temps0)/1000;
        dts[i] = dt;
        if (i < 4) stepper_NEMA17.step(2 * STEPS_PER_REV);
        delay(500);
        Serial.println(dt);
        }

    }
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
void actuatorInhale(){
    // valve piloté ouvre
    // distibuteur position inhale;
}

/*
 * This function stop the airflow
 */
void actuatorStop(){
    actuatorExhale();
}

/*
 * This function activate the actuator in the opposite direction at full speed
 */
void actuatorExhale(){
    // valve piloté ferme
    // distributeur position exhale
}

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
