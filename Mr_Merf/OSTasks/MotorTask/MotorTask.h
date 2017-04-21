/*
 * MotorTask.h
 *
 *  Created on: Apr 15, 2017
 *      Author: innakha
 */

#ifndef OSTASKS_MOTORTASK_MOTORTASK_H_
#define OSTASKS_MOTORTASK_MOTORTASK_H_

#include <stdint.h>

void MotorVelocitySet( int32_t velocityLocal );

void MotorTask( void* pvParameters );

#endif /* OSTASKS_MOTORTASK_MOTORTASK_H_ */
