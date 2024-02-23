
#ifndef _GPIO_H_
#define _GPIO_H_

#include "MKE16Z4.h"
#include <stdint.h>

#define GPIO_READ_INPUT_DATA(Register, InputBit)		(Register->PDIR & (1U << InputBit))

typedef enum {
	GPIO_DIRECTION_INPUT	= 0U,
	GPIO_DIRECTION_OUTPUT	= 1U
} GPIO_Pin_Direction_Type;

typedef struct {
	GPIO_Pin_Direction_Type direction;
	uint8_t outputLogic;
} GPIO_Config_Type;

void GPIO_Init(GPIO_Type * GPIOx, uint8_t pin, const GPIO_Config_Type* UserConfig);
uint8_t GPIO_ReadInput(GPIO_Type *gpio, const uint32_t pin);
void Delay(uint32_t n);
void App_Default_Init();
void App_Default();

#endif /* _GPIO_H_ */
