/*
 * letimer0.c
 *
 *  Created on: Sep 13, 2017
 *      Author: Vipraja
 */

#include "letimer0.h"
#include <stdbool.h>
#include "em_letimer.h"
#include "em_device.h"
#include "em_gpio.h"
#include "sleep.h"
#include "em_cmu.h"
#include "gpio.h"
#include "i2c.h"

#define freqULF 1000
#define freqLF  32768



double b;
void LETIMER0_init(void)
{
	LETIMER_Enable(LETIMER0,false);
	//GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, 0);
	double a = prescale_onduty((float)2.50);
	double b = prescale_onduty((float)0.02);
	LETIMER_CompareSet(LETIMER0, 0, a);                          /* period = 1.75secs */
	//LETIMER_CompareSet(LETIMER0, 1, a-b);                        /* on time = 20msecs */

	LETIMER_IntEnable(LETIMER0, LETIMER_IF_COMP0);

	LETIMER_IntEnable(LETIMER0, LETIMER_IF_COMP1);
	LETIMER_IntDisable(LETIMER0, LETIMER_IF_UF);


	const LETIMER_Init_TypeDef letimerInit =
	{
	.enable = false, /* Start counting when init completed*/
	.debugRun = false, /* Counter shall not keep running during debug halt. */
	.comp0Top = true, /* Load COMP0 register into CNT when counter underflows. COMP is used as TOP */
	.bufTop = false, /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
	.out0Pol = 0, /* Idle value for output 0. */
	.out1Pol = 0, /* Idle value for output 1. */
	.ufoa0 = letimerUFOANone, /* PwM output on output 0 */
	.ufoa1 = letimerUFOANone, /* No output on output 1*/
	.repMode = letimerRepeatFree /* Count while REP != 0 */
	};

	/*Intialize letimer0*/
	LETIMER_Init(LETIMER0, &letimerInit);

	LETIMER_call();

}


void LETIMER_call()
{
	blockSleepMode(LETIMER0_ENERGYMODE);

	//LETIMER_IntEnable(LETIMER0, LETIMER_IF_UF | LETIMER_IF_COMP1);
	//NVIC_ClearPendingIRQ(LETIMER0_IRQn);
	NVIC_EnableIRQ(LETIMER0_IRQn);

	LETIMER_Enable(LETIMER0, true);
}


void reset()
{
	LETIMER_CompareSet(LETIMER0, 0, prescale_onduty((float)2.50));
	//LETIMER_CompareSet(LETIMER0, 1, prescale_onduty((float)1.75)-prescale_onduty((float)0.02));

}

void increment(int *p)
{
	*p = *p +5;
	/*double newtemp = 5;
	if (newtemp < temp_default)*/
	/*int newontime = LETIMER_CompareGet(LETIMER0, 1);
	if (newontime < prescale_onduty((float)0.02))
	{
		LETIMER_CompareSet(LETIMER0, 1, 0);
	}
	else
		LETIMER_CompareSet(LETIMER0, 1, newontime-prescale_onduty((float)0.5));*/
}

void decrement(int *d)
{
	*d = *d - 5;
	/*int decontime = LETIMER_CompareGet(LETIMER0, 1);
	if(decontime > prescale_onduty(3)-prescale_onduty((float)0.5) )
	{
		LETIMER_CompareSet(LETIMER0, 1, prescale_onduty((float)1.75) );
	}
	else
		LETIMER_CompareSet(LETIMER0, 1, decontime+prescale_onduty((float)0.5));*/
}


void LETIMER0_IRQHandler(void)
{
  CORE_AtomicDisableIrq();

  if ( ((LETIMER0->IF) & (LETIMER_IF_COMP0)))
  {
	  /*GPIO_PinOutSet(LED0_port, LED0_pin);
	  LETIMER_IntClear(LETIMER0, LETIMER_IF_COMP0);*/
	  if(GPIO_PinOutGet(LED1_port,LED1_pin))
	  {
		  int newtemp = getTemperatureFromSi7021();
		  if (newtemp < tempSetValue)
		  {
			  GPIO_PinOutSet(LED1_port, LED1_pin);
		  }

	  }

  }
  else if ( ((LETIMER0->IF) & (LETIMER_IF_COMP1)))
  {
	  /*GPIO_PinOutClear(LED0_port, LED0_pin);
	   LETIMER_IntClear(LETIMER0, LETIMER_IF_COMP1);*/

  }
  else
  {

  }
 CORE_AtomicEnableIrq();
}



double prescale_onduty(double ontime)
{ double o;
	if(LETIMER0_ENERGYMODE < EM3)
		{
			o = (ontime*(double)freqLF)/2;
		}
		else
		{
			o = (ontime*(double)freqULF)/1;
		}
	return o;
	}








