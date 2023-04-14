/*
 * adcsetup.h
 *
 *  Created on: Sep 20, 2017
 *      Author: Vipraja
 */
#include <stdbool.h>

#ifndef SRC_ADCSETUP_H_
#define SRC_ADCSETUP_H_

void adc_init(void);
void adcWindow(void);
void adcWindowout(void);
void debounce(void);
void ADC0_IRQHandler(void);


#endif /* SRC_ADCSETUP_H_ */
