/*
 * adcsetup.c
 *
 *  Created on: Sep 19, 2017
 *      Author: Vipraja
 */
#include <stdio.h>
#include "em_adc.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "em_prs.h"
#include "em_rtcc.h"
#include "main.h"
#include "gpio.h"
#include "adcsetup.h"
#include "sleep.h"
#include "usart.h"
#include "timer0.h"
#include "i2c.h"

/* Definess for ADC */
#define ADC_ASYNC_CLOCK         cmuAUXHFRCOFreq_1M0Hz   /* Clock for ASYNC mode */
#define ADC_INPUT0              adcPosSelAPORT3XCH8     /* PA0 */

#define ADC_CMP_GT_VALUE        20                    /* ~0V for 3.3V AVDD */
#define ADC_CMP_LT_VALUE        3772                 /* ~3.0V for 3.3V AVDD */

volatile bool bounce = true;


void adc_init(void)
{
  ADC_Init_TypeDef adcInit = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef adcInitSingle = ADC_INITSINGLE_DEFAULT;


  /* Configure ADC */
  adcInit.ovsRateSel = adcOvsRateSel2;             // Lowest oversampling option
  adcInit.warmUpMode = adcWarmupNormal;             // ADC is not kept warmed up between conversions
  adcInit.timebase = ADC_TimebaseCalc(0);           // Calculate the ADC timebase to generate 1us warm-up time
  adcInit.prescale = 110;  // 1MHz ADC clock derived from HFCLK
  adcInit.tailgate = false;                   	     // No need for conversion tailgating
  adcInit.em2ClockConfig = adcEm2ClockAlwaysOn;
  ADC_Init(ADC0, &adcInit);

  adcInitSingle.prsSel = adcPRSSELCh0;    // Use PRS ch0
  adcInitSingle.acqTime = adcAcqTime32;    // 32 clock cycle for acquisition
  adcInitSingle.reference = adcRefVDD;    // Use buffered Vdd as reference
  adcInitSingle.resolution = adcRes12Bit; // 12-bit resolution
  adcInitSingle.diff = false;             // Use single-ended mode instead of differential
  adcInitSingle.prsEnable = false;         // Using PRS
  adcInitSingle.leftAdjust = false;       // Right-justify result
  adcInitSingle.rep = true;              // Deactivate conversion after one scan
  adcInitSingle.fifoOverwrite = true;
  adcInitSingle.posSel = ADC_INPUT0;
  ADC_InitSingle(ADC0, &adcInitSingle);

  ADC0->BIASPROG = (ADC_BIASPROG_GPBIASACC_LOWACC | ADC_BIASPROG_ADCBIASPROG_SCALE32);
  adcWindow();

  blockSleepMode(EM3);

  ADC_IntEnable(ADC0, ADC_IF_SINGLECMP);


  NVIC_EnableIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(TIMER0_IRQn);

  	  NVIC_SetPriority(ADC0_IRQn, 1);

  ADC_Start(ADC0, adcStartSingle);

}


void adcWindow(void)
{

	ADC0->SINGLECTRL |= ADC_SINGLECTRL_CMPEN;
  /* Initialize compare threshold for single conversion */
  ADC0->CMPTHR = _ADC_CMPTHR_RESETVALUE;
  ADC0->CMPTHR = (ADC_CMP_GT_VALUE << _ADC_CMPTHR_ADGT_SHIFT) + (ADC_CMP_LT_VALUE << _ADC_CMPTHR_ADLT_SHIFT);
}

void adcWindowout(void)
{
	ADC0->CMPTHR = (ADC_CMP_LT_VALUE << _ADC_CMPTHR_ADGT_SHIFT) + (ADC_CMP_GT_VALUE << _ADC_CMPTHR_ADLT_SHIFT);
}


void ADC0_IRQHandler(void)
{
	CORE_AtomicDisableIrq();

  if ((ADC0->IF & ADC_IF_SINGLECMP))
  {
	  //uint32_t sample = ADC_DataSingleGet(ADC0);
	 uint32_t samples[5] = {0,0,0,0,0};
	  samples[0] = ADC_DataSingleGet(ADC0);
	  samples[1] = ADC_DataSingleGet(ADC0);
	  samples[2] = ADC_DataSingleGet(ADC0);
	  samples[3] = ADC_DataSingleGet(ADC0);
	  samples[4] = ADC_DataSingleGet(ADC0);
	  uint32_t sample = samples[3];
	  CORE_AtomicEnableIrq();
	  if (bounce == true)
	  {
	  if( sample >= 3400 && sample <= 3520)       //north
	  {
		  BMA280_RegisterWrite(0x11, 0x00);
		 /* adcWindowout();
		  bounce = false;*/
	  }
	  else if (sample >= 2020 && sample <= 2060)    //south
	  {
		  BMA280_RegisterWrite(0x11, 0x80);         //suspend
		 /* adcWindowout();
		  bounce = false;*/

	  }
	  else if (sample >= 3110 && sample <= 3160)    //east
	  	  {
		  tempSetValue += 5;
		  adcWindowout();
		  bounce = false;
	  	  }
	  else if (sample >=2430 && sample <= 2470)    //west
	  	  {
		  tempSetValue -= 5;
		  adcWindowout();
		  bounce = false;
	  	  }
	  else if (sample >=25 && sample <= 60)        //center press
	  	  {
		  	//reset();
		  	//BMA280_RegisterWrite(0x11, 0x80);

		  	adcWindowout();
		  	bounce = false;
	  	  }
	  }
	  else if (bounce == false)
	  {
		  adcWindow();
		  bounce = true;
	  }
  }

  	  ADC0->SINGLEFIFOCLEAR = ADC_SINGLEFIFOCLEAR_SINGLEFIFOCLEAR;
  	  ADC_IntClear(ADC0, ADC_IF_SINGLECMP);
  	  CORE_AtomicEnableIrq();
}




