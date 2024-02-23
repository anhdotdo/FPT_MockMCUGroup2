
#include "MKE16Z4.h"
#include <stdint.h>

#include "UART.h"
#include "Port.h"
#include "GPIO.h"
#include "Bootloader.h"

const PortConfigType Config_BTN_PULL_UP = {
		.mux = PORT_MUX_GPIO,
		.pull = PORT_PULL_UP,
};

const GPIO_Config_Type Config_GPIO_Pin_Input = {
		.direction = GPIO_DIRECTION_INPUT
};

int main(void)
{
	PORT_Init(PORTD, 2, &Config_BTN_PULL_UP);
	GPIO_Init(GPIOD, 2, &Config_GPIO_Pin_Input);
	App_Default_Init();
	UART_Init();
	UART_StarReceiveINT();

	uint8_t appdefault = 0;

	if(GPIO_READ_INPUT_DATA(GPIOD, 2) == 0U)    /* Check that button SW3 is pressed ? */
	{
		MenuOption();
	}
	else
	{
		if (*((uint32_t *)(ADDRESS_APP_CURRENT)) == APP1_START_ADDRESS)
		{
			UART_SendString((uint8_t *)"\nApp 1 is running\n", 20);
			Bootloader_JumptoApp(APP1_START_ADDRESS);
		}
		else if (*((uint32_t *)(ADDRESS_APP_CURRENT)) == APP2_START_ADDRESS)
		{
			UART_SendString((uint8_t *)"\nApp 2 is running\n", 20);
			Bootloader_JumptoApp(APP2_START_ADDRESS);
		}
		else
		{
			appdefault = 1U;
			UART_SendString((uint8_t *)"\n------ App Default ------\n", 29);
		}
	}

    while (1)
    {
    	if (1U == appdefault)
    	{
    		App_Default();
    	}
    }
}
