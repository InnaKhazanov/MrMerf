/*
 * UltrasonicTask.h
 *
 *  Created on: Apr 15, 2017
 *      Author: innakha
 */

#ifndef OSTASKS_ULTRASONICTASK_H_
#define OSTASKS_ULTRASONICTASK_H_

#include <stdint.h>

uint32_t DistanceGet(void);

void UltasonicSensorTask(void* pvParameters);


#endif /* OSTASKS_ULTRASONICTASK_H_ */
