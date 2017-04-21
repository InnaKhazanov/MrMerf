/*
 * MotorTask.c
 *
 *  Created on: Apr 15, 2017
 *      Author: innakha
 */


#include "MotorTask.h"

#include <stdint.h>
#include <stdbool.h>

#include "../Drivers/Motor/MainMotor.h"

/* FreeRTOS include files */
#include "FreeRTOS.h"
#include "task.h"

/****************************/
/* Local Definitions        */
/****************************/

/****************************/
/* Local Functions          */
/****************************/

/****************************/
/* Local Variables          */
/****************************/
static int32_t velocity = 0;

/****************************/
/* Function Implementation  */
/****************************/

void MotorVelocitySet( int32_t velocityLocal )
{
    taskENTER_CRITICAL();
    velocity = velocityLocal;
    taskEXIT_CRITICAL();
}

void MotorTask(void* pvParameters)
{
    int32_t velocityLocal = 0;

    for( ;; )
    {
        taskENTER_CRITICAL();
        velocityLocal = velocity;
        taskEXIT_CRITICAL();

        MainMotorSetSpeedAndDirection(velocityLocal);

        vTaskDelay(1);
    }
}
