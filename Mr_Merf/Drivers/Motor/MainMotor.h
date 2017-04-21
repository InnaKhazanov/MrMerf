/*
 * HBridge.h
 *
 *  Created on: Apr 14, 2017
 *      Author: innakha
 */

#ifndef DRIVERS_H_BRIDGE_H_
#define DRIVERS_H_BRIDGE_H_

#include <stdint.h>

/* Using PWM GPIO PB4, PB5 and output GPIO PC4 */
void MainMotorInit(void);

/* Runs the Motor at the velocity in the direction specified */
/* Positive velocity runs the motor in clockwise direction, */
/* Negative velocity runs the motor in anti-clockwise direction*/
/* Velocity value must be between [-100, 100]. Given as an percentage of max capability */
void MainMotorSetSpeedAndDirection(int32_t velocity);


#endif /* DRIVERS_H_BRIDGE_H_ */
