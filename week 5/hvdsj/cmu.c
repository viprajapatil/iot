//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"
#include "em_adc.h"
#include "letimer0.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//******************er*****************************************************************
// function prototypes
//***********************************************************************************
void cmu_init(void){
	// By default, HFRCO is enabled  cmuHFRCOFreq_19M0Hz
	CMU_HFRCOBandSet(cmuHFRCOFreq_19M0Hz); 				// Set HFRCO frequency
	CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
	CMU_HFXOAutostartEnable(cmuOsc_HFXO, false, false );
	// Select Clock source
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);	// Disable HFXO


	if(LETIMER0_ENERGYMODE < EM3)
	{
		// By default, LFRCO is enabled
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);		// Enable LFXO
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

	}
	else
	{
		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);		// Enable LFXO
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

	}
	NVIC_DisableIRQ(ADC0_IRQn);
	ADC0->SINGLEFIFOCLEAR = ADC_SINGLEFIFOCLEAR_SINGLEFIFOCLEAR;
	CMU_AUXHFRCOFreqSet(cmuAUXHFRCOFreq_1M0Hz);
	CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_AUX,cmuSelect_AUXHFRCO);

	CMU->ADCCTRL = CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO;
	CMU_ClockSelectSet(cmuClock_ADC0,cmuSelect_AUXHFRCO);

	// Peripheral clocks enabled
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_LETIMER0, true);
	CMU_ClockEnable(cmuClock_ADC0, true);
	CMU_ClockEnable(cmuClock_TIMER0, true);
	//for usart
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Set the clock branch prescaler
	if(LETIMER0_ENERGYMODE < EM3)
		{
		CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_2);
		}
		else
		{
		CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_1);
		}

}



