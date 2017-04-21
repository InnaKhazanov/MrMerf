/*
 * Motor.c
 *
 *  Created on: Apr 14, 2017
 *      Author: innakha
 */

#include "MainMotor.h"

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"

/****************************/
/* Local Definitions        */
/****************************/
// Period calculation: N = (1 / f) * SysClk.
// f is the desired frequency, and SysClk is the system clock frequency.
#define PWM_PERIOD_CALC(f)     (SysCtlClockGet() / f)
#define VELOCITY_ABS(v)         ((v < 0) ? (-v) : v)

/****************************/
/* Local Functions          */
/****************************/

/****************************/
/* Local Variables          */
/****************************/
static bool hBridgeOn = false;

/****************************/
/* Function Implementation  */
/****************************/

/* Using PWM GPIO PB4, PB5 and GPIO PC4 */
void MainMotorInit(void)
{
    //Configure PWM Clock to match system
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

    // Enable the peripherals used
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlDelay(3);

    //Configure PC4 pin as output
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);

    //Configure PB4,PB5 Pins as PWM
    GPIOPinConfigure(GPIO_PB4_M0PWM2);
    GPIOPinConfigure(GPIO_PB5_M0PWM3);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 );

    //Configure PWM Options
    //PWM_GEN_1 Covers M0PWM2 and M0PWM3
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the PWM period to 250Hz. Start with duty cycle of 50%
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, PWM_PERIOD_CALC(250));
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_1) / 2);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_1) / 2);

    // Start with both PWM disabled
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, false);
    PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);

    // Enable the PWM generator
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    // Start with H-Bridge disabled
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
    hBridgeOn = false;
}


void MainMotorSetSpeedAndDirection(int32_t velocity)
{
    if( (velocity < -100) || (velocity > 100) )
    {
        return;
    }

    // if velocity is 0, stop both PWM and disable the H-Bridge
    if( velocity == 0 )
    {
        PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
        PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, false);

        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
        hBridgeOn = false;
    }
    else
    {
        uint32_t newDutyCycle = (PWMGenPeriodGet(PWM0_BASE, PWM_GEN_1)) * VELOCITY_ABS(velocity) / 100;

        if (false == hBridgeOn)
        {
            // Enable the H-Bridge
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);
            hBridgeOn = true;
        }

        if( velocity > 0 )
        {
            PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, newDutyCycle);

            PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
        }

        if( velocity < 0 )
        {
            PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, newDutyCycle);

            PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, false);
        }
    }

}
