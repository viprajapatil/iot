/*
 * letimer0.h
 *
 *  Created on: Sep 13, 2017
 *      Author: Vipraja
 */

#ifndef SRC_LETIMER0_H_
#define SRC_LETIMER0_H_
#include <stdbool.h>
#include "em_letimer.h"
#include "sleep.h"

#define LETIMER0_ENERGYMODE (EM3)

void LETIMER_call();
void LETIMER0_init();
void LETIMER0_IRQHandler();
void reset();
void increment();
void decrement();
int prescale_period(int);
double prescale_onduty(double);



#endif /* SRC_LETIMER0_H_ */

//Set Config for letimer0

