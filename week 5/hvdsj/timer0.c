/*
 * timer0.c
 *
 *  Created on: Sep 28, 2017
 *      Author: Vipraja
 */
#include "letimer0.h"
#include <stdbool.h>
#include "em_letimer.h"
#include "em_device.h"
#include "em_gpio.h"
#include "sleep.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "gpio.h"
#include "timer0.h"

#define TOP 3600
#define TIMER0_ENERGYMODE (EM1)

void TIMER0_init(void)
{
	//TIMER_Enable(TIMER0, false);
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_TIMER0, true);

	/* Select TIMER0 parameters */
	  TIMER_Init_TypeDef timerInit =
	  {
	    .enable     = false,
	    .debugRun   = true,
	    .prescale   = timerPrescale2,
	    .clkSel     = timerClkSelHFPerClk,
	    .fallAction = timerInputActionNone,
	    .riseAction = timerInputActionNone,
	    .mode       = timerModeDown,
	    .dmaClrAct  = false,
	    .quadModeX4 = false,
	    .oneShot    = true,
	    .sync       = false,

	  };

		/* Enable overflow interrupt */
		TIMER_IntEnable(TIMER0, TIMER_IF_UF);

		TIMER_IntClear(TIMER0, 0xFFFF);

		/* Set TIMER Top value */
		TIMER_CounterSet(TIMER0, TOP);
		/* Configure TIMER */
		TIMER_Init(TIMER0, &timerInit);
		NVIC_SetPriority(TIMER0_IRQn, 0);

	}


void TIMER0_delay()
{
	blockSleepMode(EM1);
	NVIC_SetPriority(TIMER0_IRQn,0);
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	/* Enable TIMER0 interrupt vector in NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);
	TIMER_Enable(TIMER0,true);

	}

void TIMER0_IRQHandler(void)
{ CORE_AtomicDisableIrq();
	/* Clear flag for TIMER0 overflow interrupt */
		  TIMER_IntClear(TIMER0, TIMER_IF_OF);

		  // Stop the timer
		  TIMER0->CMD |= TIMER_CMD_STOP;

		  unblockSleepMode(EM1);

		  CORE_AtomicEnableIrq();
}
