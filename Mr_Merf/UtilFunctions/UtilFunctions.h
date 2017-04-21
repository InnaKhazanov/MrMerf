/*
 * UtilFunctions.h
 *
 *  Created on: Apr 2, 2017
 *      Author: innakha
 */

#ifndef UTILFUNCTIONS_UTILFUNCTIONS_H_
#define UTILFUNCTIONS_UTILFUNCTIONS_H_


/* Using SysCtlDelay() */
void DelayMiliSec( unsigned int milisecToDelay, unsigned int clkF );
void DelayMicroSec( unsigned int microSecToDelay, unsigned int clkF );

/* Using timer5 */
void TimerDalayMiliSecInit(void);
void TimerDelayMiliSec(unsigned int milisecToDelay);

#endif /* UTILFUNCTIONS_UTILFUNCTIONS_H_ */
