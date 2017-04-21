/*
 * UltrasonicTask.c
 *
 *  Created on: Apr 15, 2017
 *      Author: innakha
 */

#include "UltrasonicTask.h"

#include <stdint.h>
#include <stdbool.h>

#include "../Drivers/Ultrasonic/Ultrasonic.h"
#include "../UtilFunctions/CyclicBatch/CyclicBatch.h"

/* FreeRTOS include files */
#include "FreeRTOS.h"
#include "task.h"

#include "driverlib/systick.h"

/****************************/
/* Local Definitions        */
/****************************/
#define DIST_BATCH_SIZE     MAX_BATCH_SIZE

/****************************/
/* Local Functions          */
/****************************/
static uint32_t CalcDistanceMedian();

/****************************/
/* Local Variables          */
/****************************/
static uint32_t distance = 0;

static uint32_t distanceArrayForMedianCalculation[DIST_BATCH_SIZE];

/****************************/
/* Function Implementation  */
/****************************/

uint32_t DistanceGet(void)
{
    uint32_t distanceLocal = 0;

    taskENTER_CRITICAL();
    distanceLocal = distance;
    taskEXIT_CRITICAL();

    return distanceLocal;
}


void UltasonicSensorTask(void* pvParameters)
{
    static bool firstRun = true;
    static CyclicBatchStruct distancesBatch;
    static uint32_t ditancesArray[DIST_BATCH_SIZE] = {0};

    uint32_t distanceLocal = 0;

    for( ;; )
    {
        if( true == firstRun )
        {
            CyclicBatchConstruct(&distancesBatch, DIST_BATCH_SIZE, (int32_t*)ditancesArray);
            firstRun = false;
        }

        distanceLocal = UltrasonicSensorGetDistance();

        CyclicBatchAddElement( &distancesBatch, (int32_t)distanceLocal );

        if( true == IsBatchFull(&distancesBatch) )
        {
            memcpy(distanceArrayForMedianCalculation, ditancesArray, DIST_BATCH_SIZE * sizeof(uint32_t));
            distanceLocal = CalcDistanceMedian();
        }
        else
        {
            // If the batch is not full yet don't return 0
            distanceLocal = 0;
        }

        taskENTER_CRITICAL();
        distance = distanceLocal;
        taskEXIT_CRITICAL();

        vTaskDelay(100);
    }
}



static uint32_t CalcDistanceMedian()
{
    uint32_t temp = 0;
    uint32_t i = 0, j = 0;
    uint32_t median = 0;

    // Bubble sort the array of distances in ascending order
    for( i = 0 ; i < DIST_BATCH_SIZE - 1 ; i++ )
    {
        for( j = i + 1 ; j < DIST_BATCH_SIZE ; j++)
        {
            if( distanceArrayForMedianCalculation[j] < distanceArrayForMedianCalculation[i] )
            {
                // swap elements
                temp = distanceArrayForMedianCalculation[i];
                distanceArrayForMedianCalculation[i] = distanceArrayForMedianCalculation[j];
                distanceArrayForMedianCalculation[j] = temp;
            }
        }
    }

    if( 0 == DIST_BATCH_SIZE % 2 )
    {
        // if there is an even number of elements, return mean of the two elements in the middle
        median = (distanceArrayForMedianCalculation[DIST_BATCH_SIZE/2] + distanceArrayForMedianCalculation[DIST_BATCH_SIZE/2 - 1]) / 2;
    }

    else
    {
        // else return the element in the middle
        median = distanceArrayForMedianCalculation[DIST_BATCH_SIZE/2];
    }

    return median;
}
