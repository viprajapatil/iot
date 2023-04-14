/*
 * i2c.c
 *
 *  Created on: Oct 6, 2017
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
#include "em_i2c.h"

volatile int tempSetValue = 15;

void I2C_init(void)
{
	 CMU_ClockEnable(cmuClock_HFPER, true);
	 CMU_ClockEnable(cmuClock_I2C0, true);

	 const I2C_Init_TypeDef i2cInit =
	 {
			 .enable = true,
			 .master = true,
			 .refFreq = 0,
			 .freq = I2C_FREQ_STANDARD_MAX,
			 .clhr = i2cClockHLRStandard
	 };

	 //set SDA and SCL pins
	 GPIO_PinModeSet(gpioPortC, 11, gpioModeWiredAnd, 1);
	 GPIO_PinModeSet(gpioPortC, 10, gpioModeWiredAnd, 1);


	   I2C0->ROUTELOC0 = (I2C_ROUTELOC0_SCLLOC_LOC14 | I2C_ROUTELOC0_SDALOC_LOC16);

	   I2C0->ROUTEPEN = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;

	   I2C0->CMD |= I2C_CMD_CLEARTX; //NOT SURE


	  //sensor enable
	 GPIO_PinModeSet(gpioPortD, 9, gpioModePushPull, 1);
	 GPIO_PinOutSet(gpioPortD,9);
	 I2C_Init(I2C0, &i2cInit);

	 //Sequence SCL 9 times to reset all i2c peripherals on the bus
	 for (int i = 0; i < 9; i++)
	   {
		 GPIO_PinOutClear(gpioPortC, 10);
		 GPIO_PinOutSet(gpioPortC, 10);
	   }
	 //reset the i2c bus
	  if (I2C0-> STATE & I2C_STATE_BUSY)
	   {
		   I2C0->CMD =I2C_CMD_ABORT;
	   }

	  for(int i = 0 ; i< 500000; ++i);


	  I2C_IntClear(I2C0, 0xFFFF);
	   /* Clear and enable interrupt from/ I2C module */
	   NVIC_ClearPendingIRQ(I2C0_IRQn);
	   NVIC_EnableIRQ(I2C0_IRQn);
	}


/*void I2C_RegisterRead(uint8_t addr)
{
	I2C0->TXDATA = (addr << 1) | 1;
		I2C0->CMD = I2C_CMD_START;

		//wait for the slave to respond
		while((I2C0->IFC & I2C_IF_ACK) == 0);
		I2C0->IFC = I2C_IFC_ACK;

		//read reg cmd
		I2C0->CMD |= I2C_CMD_CLEARTX;

		//wait for the slave to respond
				while((I2C0->IFC & I2C_IF_ACK) == 0);
				I2C0->IFC = I2C_IFC_ACK;

	//sr
	I2C0->TXDATA = (addr << 1) | 0;

	while((I2C0->IFC & I2C_IF_NACK) == 0);
	I2C0->IFC = I2C_IFC_NACK;

	I2C0->CMD = I2C_CMD_STOP;

//incomplete
	}*/

uint16_t I2C_RegisterWrite(uint8_t cmdaddr)
{
	uint8_t addr = 0x40;

	uint16_t temp;
	uint32_t txData = (uint32_t)(addr << 1) & 0xFE;

	I2C0->TXDATA = txData;
	I2C0->CMD = I2C_CMD_START;

	//wait for the slave to respond
	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC = I2C_IFC_ACK;

	I2C0->TXDATA = cmdaddr;

	//wait for the slave to respond
		while((I2C0->IF & I2C_IF_ACK) == 0);
		I2C0->IFC = I2C_IFC_ACK;

	I2C0->CMD = I2C_CMD_START;

	I2C0->TXDATA = (addr << 1) | 0x01;

	//wait for the slave to respond
		while((I2C0->IF & I2C_IF_ACK) == 0);
		I2C0->IFC = I2C_IFC_ACK;

		while((I2C0->IF & I2C_IF_RXDATAV) == 0);
    //ms byte
		uint32_t tempRXData = I2C0->RXDATA;
		temp = (uint16_t)tempRXData;
		temp = temp << 8;

		//wait for the slave to respond
		//pwhile((I2C0->IFC & I2C_IF_ACK) == 0);
			I2C0->CMD = I2C_CMD_ACK;

			while((I2C0->IF & I2C_IF_RXDATAV) == 0);
		//ls byte
			tempRXData = I2C0->RXDATA;
			temp |= (uint16_t)tempRXData;

		 //while((I2C0->IF & I2C_IF_NACK) == 0);
		  //I2C0->IFC = I2C_IFC_NACK;
			I2C0->CMD = I2C_CMD_NACK;
		//stop condition
		I2C0->CMD = I2C_CMD_STOP;

		return temp;
	}


/*void enablegpioSi7021()
{
		GPIO_IntConfig(gpioPortD, 9,true,false, true);
		GPIO_IntClear(GPIO_IFC_EXT_DEFAULT);
		GPIO_IntEnable(gpioPortD);
		NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
		NVIC_EnableIRQ(GPIO_ODD_IRQn);

}*/

int getTemperatureFromSi7021()
{
  uint32_t convert;
  uint16_t tempCode = I2C_RegisterWrite(0xe3);
  convert = (tempCode * 17572)>>16;
  convert = convert - 4685;
  convert = ((convert)>>2)/25;

  return convert;
}
