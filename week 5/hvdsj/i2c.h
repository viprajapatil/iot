/*
 * i2c.h
 *
 *  Created on: Oct 7, 2017
 *      Author: Vipraja
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

volatile int tempSetValue;


typedef enum
{
  tempsensRegTemp =       0,   /**< Temperature register (read-only) */
  tempsensRegConfig =     1,   /**< Configuration register */
  tempsensRegHysteresis = 2,   /**< Hysteresis register */
  tempsensRegShutdown =   3    /**< Overtemperature shutdown register */
}TEMPSENS_Register_TypeDef;


void I2C_init();
uint16_t I2C_RegisterWrite(uint8_t);
//void I2C_RegisterRead();
void I2C0_IRQHandler();
int getTemperatureFromSi7021();

#endif /* SRC_I2C_H_ */
