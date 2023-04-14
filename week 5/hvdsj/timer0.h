/*
 * timer0.h
 *
 *  Created on: Sep 28, 2017
 *      Author: Vipraja
 */

#ifndef SRC_TIMER0_H_
#define SRC_TIMER0_H_

#include "em_timer.h"
#define TIMER0_ENERGYMODE (EM1)

void TIMER0_init();
void TIMER0_delay();
void TIMER0_IRQHandler();

#endif /* SRC_TIMER0_H_ */
