/*
 * usart.c
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
#include "em_cmu.h"
#include "gpio.h"
#include "em_usart.h"
#include "usart.h"



void usart_init()
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	USART_InitSync_TypeDef initSync =
	{
			.enable = usartDisable,
			.refFreq = 0,
			.baudrate = 1000000,
			.databits = usartDatabits8,
			.master = true, /* Operate as SPI master */
			.msbf = true,
			.clockMode = usartClockMode0, /* Clock idle low, sample on falling edge. */
			.prsRxEnable = false,
			.prsRxCh =usartPrsRxCh0,
			.autoTx = false,
			.autoCsEnable = true,
			.autoCsHold = 1,
			.autoCsSetup = 1
	};

	USART_InitSync(USART1, &initSync);
}

void usart_setup()
{
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(gpioPortD, 11 , gpioModeInput, 1);

	/*cmu_init_USARTn(USART1);
	gpio_SPI_init();*/
	GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 1);  //CS
	GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 1);  //SCK
	GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 0);     //MISO
	GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 1);  //MOSI

	usart_init();

	/* Routing the LOC0 to appropriate Pins */
	USART1->ROUTELOC0 = (USART1->ROUTELOC0 &
							~(	_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK |
								_USART_ROUTELOC0_CLKLOC_MASK| _USART_ROUTELOC0_CSLOC_MASK
							)
						 )
						 | (11 << _USART_ROUTELOC0_TXLOC_SHIFT)
						 | (11 << _USART_ROUTELOC0_RXLOC_SHIFT)
						 | (11 << _USART_ROUTELOC0_CLKLOC_SHIFT)
						 | (11 <<_USART_ROUTELOC0_CSLOC_SHIFT);

	/*Routing enable TX, RX, CLK, and CS pin for USART1 use */
	USART1->ROUTEPEN = (USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_CLKPEN );

	USART1->CMD |= USART_CMD_CLEARTX | USART_CMD_CLEARRX;

	/*USART0->ROUTELOC0 = USART_ROUTELOC0_CLKLOC_LOC11 | USART_ROUTELOC0_TXLOC_LOC11 | USART_ROUTELOC0_RXLOC_LOC11 | USART_ROUTELOC0_CSLOC_LOC11;
	USART0->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CSPEN;
*/
	USART_Enable(USART1, usartEnable);
	}

/*
	 * The SPI frame consists of 16 bits and the first bit is a read/write selector (1=read, 0 = write). The R/W bit is followed by a 7-bit address field.
	 * The lower 8 bits are used for data. In case of write operation, the data is written by host (i.e. BGM111). In case of read, the data is written by
	 * the sensor (BMA280).*/
uint8_t BMA280_RegisterRead(uint8_t addr)
{
USART1->CMD |= USART_CMD_CLEARTX | USART_CMD_CLEARRX;

uint32_t dataRead;
dataRead = (1 << 7) | addr;

/* Waiting for the USART1 to be ready */
while (!(USART_StatusGet(USART1) & USART_STATUS_TXBL));

//Sending Dummy Data to generate the clock and send the register address to read data from
USART1->TXDOUBLE = dataRead;

/* Wait for TX to complete */
while(!(USART_StatusGet(USART1)& USART_STATUS_TXC));

// Wait for valid RX DATA
while(!(USART_StatusGet(USART1)& USART_STATUS_RXDATAV));

dataRead = (USART1->RXDOUBLE);
dataRead = dataRead >>8;

return dataRead;
}

void BMA280_RegisterWrite(uint8_t addr, uint8_t data)
{
	USART1->CMD |= USART_CMD_CLEARTX | USART_CMD_CLEARRX;

		uint32_t dataToSend;

		//dataToSend = (0 << 7) | addr;
		dataToSend = (uint32_t)(addr | (data << 8));
		/* Waiting for the USART1 to be ready */
		while (!(USART1->STATUS & USART_STATUS_TXBL));

		USART1->TXDOUBLE =	dataToSend;

		/* Wait for TX to complete */
		while(!(USART1->STATUS & USART_STATUS_TXC));

		//(USART1->RXDOUBLE);
	}

 void enablegpio()
{
		GPIO_IntConfig(gpioPortD, 11,true,false, true);
		GPIO_IntClear(GPIO_IFC_EXT_DEFAULT);
		GPIO_IntEnable(gpioPortD);
		NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
		NVIC_EnableIRQ(GPIO_ODD_IRQn);

}

void BMA280_init(void)
{

	BMA280_RegisterWrite(0x16, (0x20 | 0x10)); //single tap - bit 5
	BMA280_RegisterRead(0x16);
	//BMA280_RegisterWrite(0x16, 0x10); //double tap - bit 4
	//BMA280_RegisterRead(0x16);
	BMA280_RegisterWrite(0x2b, 0x42); //tap_thershold 250mg, tap_samples 4
	BMA280_RegisterRead(0x2b);
	BMA280_RegisterWrite(0x2a, 0x03); //tap_shock 50ms ,tap_duartion 200ms, tap_quiet 30ms
	BMA280_RegisterRead(0x2a);
	BMA280_RegisterWrite(0x10, 0x0c); //bandwidth 125Hz
	BMA280_RegisterRead(0x10);
	BMA280_RegisterWrite(0x0f, 0x05); //range 4g
	BMA280_RegisterRead(0x0f);
	BMA280_RegisterWrite(0x19, 0x30); //map single and double tap to int1
	BMA280_RegisterRead(0x19);
	BMA280_RegisterWrite(0x1e, 0x10);
	BMA280_RegisterRead(0x1e);
	BMA280_RegisterWrite(0x21, 0x0c);
	BMA280_RegisterRead(0x21);
	BMA280_RegisterWrite(0x20, 0x01);
	BMA280_RegisterRead(0x20);

	enablegpio();

	BMA280_RegisterWrite(0x11, 0x80); //suspend


}

void GPIO_ODD_IRQHandler()
{
		CORE_AtomicDisableIrq();

		unsigned int pin = GPIO_PinOutGet(gpioPortD,9);

			GPIO_IntClear(0xFFFF);

			if(pin == 0)
			{
				GPIO_PinModeSet(gpioPortD, 9, gpioModeInput, 1); //sensor enable
				GPIO_PinModeSet(gpioPortC, 11, gpioModeWiredAnd, 1); //sda
			    GPIO_PinModeSet(gpioPortC, 10, gpioModeWiredAnd, 1); //scl

			    //Sequence SCL 9 times to reset all i2c peripherals on the bus
			    	 for (int i = 0; i < 9; i++)
			    	   {
			    	     GPIO_PinOutClear(gpioPortC, 10);
			    	     GPIO_PinOutSet(gpioPortC, 10);
			    	   }

			    if (I2C_STATE_BUSY)
			    {
			    	I2C0->CMD = I2C_CMD_ABORT;
			    }

			    //INTIALIZE Si7021

			    //I2C_RegisterWrite(0xe3);

				BMA280_RegisterWrite(0x16, (0x00 << 5)  | (0x01 << 4));		//enabling double tap Interrupt
				BMA280_RegisterRead(0x16);
				BMA280_RegisterWrite(0x19, 0x01 << 4); 	//mapping INT1 to double TAP
				BMA280_RegisterRead(0x19);
			}
			else if (pin == 1)
			{
				//disable function to get temp measurement

				//disable sda scl pins
				GPIO_PinModeSet(gpioPortC, 11, gpioModeDisabled, 0); //sda
				GPIO_PinModeSet(gpioPortC, 10, gpioModeDisabled, 0); //scl

				GPIO_PinOutClear(gpioPortD, 9);

				BMA280_RegisterWrite(0x16, (0x01 << 5) | (0x00 << 4) );		//enabling single tap Interrupt
				BMA280_RegisterRead(0x16);
				BMA280_RegisterWrite(0x19, 0x01 << 5); 	//mapping INT1 to single TAP
				BMA280_RegisterRead(0x19);
			}

		CORE_AtomicEnableIrq();

	}

