//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define JOYSTICK_PORT   gpioPortA
#define JOYSTICK_PIN    0
#define ADC_INPUT0      adcPosSelAPORT3XCH8     /* PA0 */

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************
void gpio_init(void){

	// Set LED ports to be standard output drive with default off (cleared)
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);


	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	GPIO_PinModeSet(JOYSTICK_PORT, 0 , gpioModeDisabled, 0);
	GPIO_PinModeSet(gpioPortD, 11 , gpioModePushPull, 0);
}
