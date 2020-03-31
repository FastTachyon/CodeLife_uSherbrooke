//
// Created by xaviert on 2020-03-28.
//

#ifndef CODELIFE_USHERBROOKE_ACTUATOR_H
#define CODELIFE_USHERBROOKE_ACTUATOR_H

#include <Stepper.h>



#define PNEUMATIC
//#define MECANIC

/*
 * This function modulate the inhale flow
 * input : inhale flow value ml/sec
 * output: modulation
 */
void speedControl(double);

/*
 * This function activate the actuator to push air
 */
void actuatorInhale();

/*
 * This function stop the airflow
 */
void actuatorStop();

/*
 * This function activate the actuator in the opposite direction at full speed
 */
void actuatorExhale();

/*
 * This function calibrate the piston's speed
 */
void Calibrate_motor();

#endif //CODELIFE_USHERBROOKE_ACTUATOR_H
