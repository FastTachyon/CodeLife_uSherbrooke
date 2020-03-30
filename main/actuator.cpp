//
// Created by xaviert on 2020-03-28.
//

#include "actuator.h"

#ifdef PNEUMATIC
/*
#include <Servo.h>

Servo flowControl;
int servoPin = 9;

flowControl.attach(servoPin);
*/
/*
 * This function modulate the inhale flow
 * input : inhale flow value ml/sec
 * output: modulation
 */
 
void speedControl(double flow){
    // calcul du flow d'air en fcn du flow demandé
    // map du flow pour le modulateur de débit
    int angle; //(0-180)
//    flowControl.write(angle);
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
#include <Stepper.h>

const int STEPS_PER_REV = 200;

Stepper stepper_NEMA17(STEPS_PER_REV, 8, 9, 10, 11);

const int STEPPER_MAX_SPEED = 200;

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
