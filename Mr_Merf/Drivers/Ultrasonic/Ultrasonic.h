/*
 * Ultrasonic.h
 *
 *  Created on: Mar 26, 2017
 *      Author: innakha
 */

#ifndef DRIVERS_ULTRASONIC_H_
#define DRIVERS_ULTRASONIC_H_

#include <stdint.h>

/* Using GPIO PORT D - 0,1 */
void UltrasonicSensorInit(void);

/* Get the distance an object in millimeters */
uint32_t UltrasonicSensorGetDistance(void);

/* Testing function */
float UltasonicSensorTest(void);

#endif /* DRIVERS_ULTRASONIC_H_ */
