
#include "GPIO.h"

void GPIO_Init(GPIO_Type * GPIOx, uint8_t pin, const GPIO_Config_Type* UserConfig)
{
	if (GPIO_DIRECTION_INPUT == UserConfig->direction)
	{
		GPIOx->PDDR &= ~(1U << pin);
	}
	else if (GPIO_DIRECTION_OUTPUT == UserConfig->direction)
	{
		GPIOx->PDDR |= GPIO_DIRECTION_OUTPUT << pin;

		if (0U == UserConfig->outputLogic)
		{
			GPIOx->PCOR = (1U << pin);
		}
		else
		{
			GPIOx->PSOR = (1U << pin);
		}
	}
}

uint8_t GPIO_ReadInput(GPIO_Type *gpio, const uint32_t pin)
{
    return (((gpio->PDIR) >> pin) & 0x01);
}

void Delay(uint32_t n)
{
	while(n--);
}

void App_Default_Init()
{
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1U);
	PORTB->PCR[5] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[5] |= PORT_PCR_MUX(1);
	GPIOB->PDDR |= 1U << 5;
	GPIOB->PDOR = 1U << 5;
}

void App_Default()
{
	GPIOB->PTOR = 1U << 5;
	Delay(5000000);
}
