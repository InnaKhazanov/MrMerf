/*
 * ControllerTask.c
 *
 *  Created on: Apr 15, 2017
 *      Author: innakha
 */


#include "ControllerTask.h"

#include <stdint.h>
#include <stdbool.h>

// Other OS Tasks includes
#include "UltrasonicTask/UltrasonicTask.h"
#include "MotorTask/MotorTask.h"

/* FreeRTOS include files */
#include "FreeRTOS.h"
#include "task.h"

/****************************/
/* Local Definitions        */
/****************************/
#define CRUISE_SPEED                50
#define SPEED_STEP                  1

// all distances should be in mm
#define MIN_DIST_TO_GO_BACK         100
#define MIN_DIST_TO_GO_FORWARD      200
#define MIN_DIST_TO_STOP            50
#define MIN_DIST_TO_START           100


typedef enum {
    GO_FORWARD,
    GO_BACKWARD,
    STOP

} MovingStateEnum;

/****************************/
/* Local Functions          */
/****************************/
static int32_t CalculateNextVelocity( uint32_t distance );
static int32_t SpeedTransient( int32_t finalSpeed );
static MovingStateEnum StateMachineGetNextState( MovingStateEnum prevState, uint32_t distance );
static int32_t GetNextDestinationSpeed( MovingStateEnum state );

/****************************/
/* Local Variables          */
/****************************/

/****************************/
/* Function Implementation  */
/****************************/

void ContollerLogicTask( void* pvParameters )
{
    uint32_t distance = 0;
    int32_t velocity = 0;

    for( ;; )
    {
        distance = DistanceGet();

        velocity = CalculateNextVelocity(distance);

        MotorVelocitySet(velocity);

        vTaskDelay(10);
    }
}


static int32_t CalculateNextVelocity( uint32_t distance )
{
    static MovingStateEnum prevState = STOP;
    MovingStateEnum nextState;
    int32_t velocityWanted = 0;
    int32_t velocityCalulated = 0;

    nextState = StateMachineGetNextState(prevState, distance);

    velocityWanted = GetNextDestinationSpeed(nextState);

    velocityCalulated = SpeedTransient(velocityWanted);

    // Update the prev state for the next cycle
    prevState = nextState;

    return velocityCalulated;
}


static MovingStateEnum StateMachineGetNextState( MovingStateEnum prevState, uint32_t distance )
{
    MovingStateEnum nextState;

    switch(prevState)
    {
    case(STOP):
            if( distance > MIN_DIST_TO_START )
            {
                nextState = GO_FORWARD;
            }
            else
            {
                if( distance > MIN_DIST_TO_STOP)
                {
                    nextState = GO_BACKWARD;
                }
                else
                {
                    nextState = STOP;
                }
            }
            break;

    case(GO_FORWARD):
                if( distance > MIN_DIST_TO_GO_BACK )
                {
                    nextState = GO_FORWARD;
                }
                else
                {
                    if( distance < MIN_DIST_TO_STOP )
                    {
                        nextState = STOP;
                    }
                    else
                    {
                        nextState = GO_BACKWARD;
                    }
                }
                break;

    case(GO_BACKWARD):
                 if( distance > MIN_DIST_TO_GO_FORWARD )
                 {
                     nextState = GO_FORWARD;
                 }
                 else
                 {
                     if( distance < MIN_DIST_TO_STOP )
                     {
                         nextState = STOP;
                     }
                     else
                     {
                         nextState = GO_BACKWARD;
                     }
                 }
            break;
    default:
        break;
    }

    return nextState;
}


static int32_t GetNextDestinationSpeed( MovingStateEnum state )
{
    int32_t velocityWanted = 0;

    switch(state)
      {
      case(STOP):
              velocityWanted = 0;
              break;
      case(GO_FORWARD):
              velocityWanted = CRUISE_SPEED;
              break;
      case(GO_BACKWARD):
              velocityWanted = -CRUISE_SPEED;
              break;
      default:
              break;
      }

    return velocityWanted;
}


static int32_t SpeedTransient( int32_t finalSpeed )
{
    static int32_t prevSpeed = 0;
    int32_t nextSpeed = 0;

    if (prevSpeed == finalSpeed)
    {
        nextSpeed = prevSpeed;
    }
    else
    {
        if(prevSpeed < finalSpeed)
        {
            nextSpeed = prevSpeed + SPEED_STEP;
        }
        else
        {
            nextSpeed = prevSpeed - SPEED_STEP;
        }
    }

    // Before exiting save the next speed for the next cycle
    prevSpeed = nextSpeed;

    return nextSpeed;
}
