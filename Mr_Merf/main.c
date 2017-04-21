/*
 * main.c
 */


//#include "inc/tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

// Driver lib include files
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/pwm.h"

// Local drivers and utilities
#include "Drivers/UART/uartstdio.h"
#include "Drivers/UART/UARTInit.h"
#include "Drivers/Ultrasonic/Ultrasonic.h"
#include "UtilFunctions/UtilFunctions.h"
#include "Drivers/Motor/MainMotor.h"


// FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// OS Tasks includes
#include "OSTasks/UltrasonicTask/UltrasonicTask.h"
#include "OSTasks/MotorTask/MotorTask.h"
#include "OSTasks/ControllerTask/ControllerTask.h"

/****************************/
/* Local Definitions        */
/****************************/
/* Task priorities */
#define PRINT_A_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )
#define PRINT_B_TASK_PRIORITY       ( tskIDLE_PRIORITY + 2 )

/****************************/
/* Local Functions          */
/****************************/
static void Init(void);

static void PrintATask( void* pvParameters );
static void PrintBTask( void* pvParameters );

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
    uint32_t counter = 0;
    void* dontCareParams = NULL;

    Init();

    /****************** FreeRTOS tasks init ******************/
    xTaskCreate( UltasonicSensorTask, "USNC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate( MotorTask, "MTR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate( ContollerLogicTask, "CNTR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    /****************** FreeRTOS tasks run ******************/
    vTaskStartScheduler();
}

