//***********************************************************************************
// Include files
//***********************************************************************************
#include "main.h"
#include "em_gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED0 pin
#define	LED0_port 5
#define LED0_pin 4
#define LED0_default	false 	// off
// LED1 pin
#define LED1_port 5
#define LED1_pin 5
#define LED1_default	false	// off
// Joystick
#define JOYSTICK_PORT   gpioPortA
#define JOYSTICK_PIN    0
#define ADC_INPUT0      adcPosSelAPORT3XCH8     /* PA0 */


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_init(void);

