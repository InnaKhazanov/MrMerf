/*
 * main.c
 */


//#include "inc/tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

// Driverlib include files
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"

// Local drivers and utilities
#include "Drivers/UART/UARTInit.h"
#include "Drivers/Ultrasonic/Ultrasonic.h"
#include "UtilFunctions/UtilFunctions.h"
#include "Drivers/Motor/MainMotor.h"


// FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"

// Tasks includes
#include "OSTasks/UltrasonicTask/UltrasonicTask.h"
#include "OSTasks/MotorTask/MotorTask.h"
#include "OSTasks/ControllerTask/ControllerTask.h"

/****************************/
/* Local Definitions        */
/****************************/

/****************************/
/* Local Functions          */
/****************************/
static void Init(void);

/****************************/
/* Local Variables          */
/****************************/

/****************************/
/* Function Implementation  */
/****************************/

static void Init(void)
{
    // Set the clocking to run directly from the crystal at 16MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    /****************** UART init ******************/
    UARTInit();

    /****************** Ultrasonic driver init ******************/
    UltrasonicSensorInit();

    /****************** Motors driver init ******************/
    MainMotorInit();

    /****************** Timer for delay util init ******************/
    //TimerDalayMiliSecInit();

    /****************** SysTick for FreeRTOS init ******************/
    SysTickPeriodSet(SysCtlClockGet()/1000); //1ms
    IntMasterEnable();
    SysTickIntEnable();
    SysTickEnable();

}



int main(void)
{
    Init();

    /****************** FreeRTOS tasks init ******************/
    xTaskCreate( UltasonicSensorTask, "USNC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate( MotorTask, "MTR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate( ContollerLogicTask, "CNTR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    /****************** FreeRTOS tasks run ******************/
    vTaskStartScheduler();
}

