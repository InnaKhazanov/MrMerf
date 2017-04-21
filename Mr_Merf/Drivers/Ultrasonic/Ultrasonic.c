/*
 * Ultrasonic.c
 *
 *  Created on: Mar 26, 2017
 *      Author: innakha
 */

#include "Ultrasonic.h"

#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#include "../../UtilFunctions/UtilFunctions.h"
#include "../UART/uartstdio.h"

/****************************/
/* Local Definitions        */
/****************************/
#define CLK_F           16000000
#define MAX_COUNTER     7500    /* max distance detection 127.5cm */

#define SOUND_SPEED_MM_NANO_SEC    (0.034 * 10 / 1000)
#define TICK_NANO_SEC              62.5
#define MM_PER_TICK                (SOUND_SPEED_MM_NANO_SEC * TICK_NANO_SEC)

/****************************/
/* Local Functions          */
/****************************/
static void SendTriggerSignal(void);
static uint32_t GetEchoSignel(void);
static void EchoPinIntHanbler(void);    //interrupt handler

/****************************/
/* Local Variables          */
/****************************/
static volatile uint32_t timerCounter = 0;
static volatile bool timerCounterReadyFlag = false;

/****************************/
/* Function Implementation  */
/****************************/

void UltrasonicSensorInit(void)
{
    // Enable the peripherals - PORT D, Wide Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
    SysCtlDelay(3);

    //set the PORTD pins: pin_0 as input, pin_1 as output
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);

    //initialize the interrupts for the rising/falling edge of the echo pin PD1
    GPIOIntDisable(GPIO_PORTD_BASE, GPIO_PIN_0);        // Disable interrupt on PD1 in case it was enabled
    GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_0);          // Clear pending interrupts for PD1 in case it was pending

    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_BOTH_EDGES | GPIO_DISCRETE_INT);
    GPIOIntRegister(GPIO_PORTD_BASE, EchoPinIntHanbler);
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_0);

    // initialize the wide timer 0 for the echo pin
    // configure: 32-bit periodic timer, counts up from zero
    TimerConfigure(WTIMER0_BASE, TIMER_CFG_A_ONE_SHOT_UP);
    TimerLoadSet(WTIMER0_BASE, TIMER_A, 24*(SysCtlClockGet() / 1000) - 1); //load of 24 milisec (~4m on the echo pin)

    TimerDisable(WTIMER0_BASE, TIMER_A);

}


uint32_t UltrasonicSensorGetDistance(void)
{
    SendTriggerSignal();
    return GetEchoSignel();
}


static void SendTriggerSignal(void)
{
    /***  Send a trigger on the TRIG pin ***/
    //Clear the trigPin
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
    DelayMicroSec(3, CLK_F);

    //Set the trigPin on HIGH state for at least 10 micro seconds
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    DelayMicroSec(11, CLK_F);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
}


static uint32_t GetEchoSignel(void)
{
    float distance = 0;

    while( false == timerCounterReadyFlag )
    {
        //busy wait. Do nothing
    }

    // distance in millimeter. Divide by 2 for double the distance
    distance = timerCounter * MM_PER_TICK / 2;
    timerCounterReadyFlag = false;

    return (uint32_t)distance;
}



static void EchoPinIntHanbler(void)
{
    uint32_t status = 0;
    int32_t gpioRead = 0;

    status = GPIOIntStatus(GPIO_PORTD_BASE, true);
    GPIOIntClear(GPIO_PORTD_BASE, status);

    //rising edge detected
    gpioRead =  GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) & 0x1;
    if(0 != gpioRead)
    {
        TimerDisable(WTIMER0_BASE, TIMER_A);
        TimerEnable(WTIMER0_BASE, TIMER_A);
        timerCounter = TimerValueGet(WTIMER0_BASE, TIMER_A);
    }

    //falling edge detected
    else
    {
        timerCounter = TimerValueGet(WTIMER0_BASE, TIMER_A);
        timerCounterReadyFlag = true;

    }

}


//*****************************************************************************
// DESCRIPTION: simple implementation for testing the sensor. No interrupts
//*****************************************************************************
float UltasonicSensorTest(void)
{
    uint32_t counter = 0U;

    /***  Send a trigger on the TRIG pin ***/
    //Clear the trigPin
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
    DelayMicroSec(2, CLK_F);

    //Set the trigPin on HIGH state for at least 10 micro seconds
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    DelayMicroSec(11, CLK_F);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);


    /***  Read the Echo pin and calculate distance ***/
    // wait for the echo pin HIGH value
    while(0 == (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) & 0x1)) {}

    // wait for the echo pin LOW value and count the milisec
    while( (0 != (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) & 0x1)) && (counter < MAX_COUNTER))
    {
        counter++;
        DelayMicroSec(1, CLK_F);
    }

   UARTprintf("count = %d\n", counter);

    return counter * 0.017000;   //(0.034 / 2 => speed of sound divided by 2)
}


