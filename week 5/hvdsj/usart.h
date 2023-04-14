/*
 * usart.h
 *
 *  Created on: Sep 28, 2017
 *      Author: Vipraja
 */

#ifndef SRC_USART_H_
#define SRC_USART_H_

void usart_init();
void BMA280_RegisterWrite(uint8_t, uint8_t);
uint8_t BMA280_RegisterRead(uint8_t);
void BMA280_suspend();
void BMA280_init();
void enablegpio();

int t = 15;

#endif /* SRC_USART_H_ */
