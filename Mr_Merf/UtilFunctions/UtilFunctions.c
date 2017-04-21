/*
 * UtilFunctions.c
 *
 *  Created on: Apr 2, 2017
 *      Author: innakha
 */
#include "inc/tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>
#include "UtilFunctions.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


/****************************/
/* Local Functions          */
/****************************/
static void MilisecTimer5Int(void);


/****************************/
/* Local Variables          */
/****************************/
static volatile unsigned int timerDelayMilisecCount;


/****************************/
/* Function Implementation  */
/****************************/

//*****************************************************************************
// DESCRIPTION: performs a delay for a given number of milisec
// LOGIC: T = (1 / f) * 3 * 1000
// where:   T = time in milisec of one call to ROM_SysCtlDelay
//          f = clock frequency
//          3 is the number of instructions performed in one call to  ROM_SysCtlDelay
//          1000 for conversion to milisec
//       numDelayLoops = milisecToDelay / T
//*****************************************************************************
void DelayMiliSec( unsigned int milisecToDelay, unsigned int clkF )
{
    double numDelayLoops = (milisecToDelay * clkF) / (3 * 1000);

    SysCtlDelay((unsigned int)numDelayLoops);
}


//*****************************************************************************
// DESCRIPTION: performs a delay for a given number of microsec
// LOGIC: T = (1 / f) * 3 * 1000000
// where:   T = time in microsec of one call to ROM_SysCtlDelay
//          f = clock frequency
//          3 is the number of instructions performed in one call to  ROM_SysCtlDelay
//          1000000 for conversion to microsec
//       numDelayLoops = milisecToDelay / T
//*****************************************************************************
void DelayMicroSec( unsigned int microSecToDelay, unsigned int clkF )
{
    double numDelayLoops = (microSecToDelay * clkF) / (3 * 1000000);

    SysCtlDelay((unsigned int)numDelayLoops);
}



//*****************************************************************************
// DESCRIPTION: initializes the timer 5 needed for the TimerDelayMiliSec functions
//*****************************************************************************
void TimerDalayMiliSecInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
    SysCtlDelay(3);

    /* Configure: 16-bit periodic timer, counts down from load to zero and wraps around zero */
    TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);

    /**************** Init of the 1 Milisec timer ****************/
    TimerLoadSet(TIMER5_BASE, TIMER_A, (SysCtlClockGet() / 1000) - 1);

    /* Enable and register the interrupt function for the timer */
    TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER5_BASE, TIMER_A, MilisecTimer5Int);          //also enables global interrupts

    TimerEnable(TIMER5_BASE, TIMER_A);
    timerDelayMilisecCount = 0;

}

//*****************************************************************************
// DESCRIPTION: performs a delay for a given number of milisec using timer 5
//*****************************************************************************
void TimerDelayMiliSec(unsigned int milisecToDelay)
{
    volatile unsigned int temp = timerDelayMilisecCount;
    while ( (timerDelayMilisecCount - temp) < milisecToDelay)
    {
        // do nothing
    }
}


//*****************************************************************************
// DESCRIPTION: interrupt handler of timer 5 used for the MilisecTimerDelay functions
//*****************************************************************************
static void MilisecTimer5Int(void)
{
    unsigned int status = 0;

    status = TimerIntStatus(TIMER5_BASE, true);
    TimerIntClear(TIMER5_BASE,status);

    timerDelayMilisecCount++;
}
