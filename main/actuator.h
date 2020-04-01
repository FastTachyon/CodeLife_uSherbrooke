//
// Created by xaviert on 2020-03-28.
//

#ifndef CODELIFE_USHERBROOKE_ACTUATOR_H
#define CODELIFE_USHERBROOKE_ACTUATOR_H

#include <Stepper.h>



#define PNEUMATIC
//#define MECANIC

#ifdef PNEUMATIC
/*
 * This function calibrate the piston's speed
 */
void Calibrate_pneu();
#elif def MECANIC
void Calibrate_meca();
#endif

/*
 * This function activate the driver for the stepper motor
 */
void enableStepper();

/*
 * This function disable the driver for the stepper motor
 */
void disableStepper();

/*
 * This function modulate the actuator speed during inhale
 * input : inspiration time
 * output: modulation
 */

void speedControl(double);


/*
 * This function stop the airflow
 */
void actuatorStop();


#endif //CODELIFE_USHERBROOKE_ACTUATOR_H
