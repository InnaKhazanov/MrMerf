/*
 * UARTInit.c
 *
 *  Created on: Apr 16, 2017
 *      Author: innakha
 */

#include "UARTInit.h"

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "uartstdio.h"

/****************************/
/* Local Definitions        */
/****************************/

/****************************/
/* Local Functions          */
/****************************/

/****************************/
/* Local Variables          */
/****************************/

/****************************/
/* Function Implementation  */
/****************************/


void UARTInit(void)
{
    // Enable the peripherals used by UART.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(3);

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // run the UART-util init function
    UARTStdioConfig(0, 9600, SysCtlClockGet());
}
