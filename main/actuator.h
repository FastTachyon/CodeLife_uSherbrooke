//
// Created by xaviert on 2020-03-28.
//

#ifndef CODELIFE_USHERBROOKE_ACTUATOR_H
#define CODELIFE_USHERBROOKE_ACTUATOR_H

#include <Stepper.h>

const int STEPS_PER_REV = 200;
Stepper stepper_NEMA17(STEPS_PER_REV, 22, 23, 24, 25);
const int STEPPER_MAX_SPEED = 200;
int stepperPosition = 0;

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
