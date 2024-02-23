
#include "Bootloader.h"
#include "MKE16Z4.h"

void Bootloader_JumptoApp(const uint32_t Address)
{
	uint32_t App_ResetHandler_Address;

	// Data Memory Barrier
	__DMB();

	// Disable all configurable interrupts and turn off Systick
	__disable_irq();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;

	// Set Address
	SCB->VTOR = Address;

	// Set Main Stack Pointer
	__set_MSP(*((volatile uint32_t*)Address));

	// Calculate App_ResetHandler_Address = Address + 4
	App_ResetHandler_Address = *((volatile uint32_t*)(Address + 4U));

	// Set Program Counter to App_ResetHandler_Address
	pFunction_t JumptoApp = (pFunction_t)App_ResetHandler_Address;

	//Jump to App
	JumptoApp();
}
