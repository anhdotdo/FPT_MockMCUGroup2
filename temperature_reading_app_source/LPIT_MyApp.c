#include "LPIT_MyApp.h"

// function definition
// LPIT_Init
void LPIT_Init()
{
	// Clock configure - asyn clock
	PCC->CLKCFG[PCC_LPIT0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;	// disable bus clock

	SCG->SIRCDIV &= ~SCG_SIRCDIV_SIRCDIV2_MASK;
	SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV2(4U);				// divide 8
	PCC->CLKCFG[PCC_LPIT0_INDEX] &= ~PCC_CLKCFG_PCS_MASK;
	PCC->CLKCFG[PCC_LPIT0_INDEX] |= PCC_CLKCFG_PCS(2U);		// slow IRC = 8MHz

	// bus clock
	PCC->CLKCFG[PCC_LPIT0_INDEX] |= PCC_CLKCFG_CGC(1U);

	// update system core clock
	SystemCoreClockUpdate();

	// Reset the timer channels and registers;
	LPIT0->MCR |= LPIT_MCR_SW_RST(1U);
	LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;

	// Setup timer operation in debug and doze modes and enable the module
	LPIT0->MCR |= (LPIT_MCR_DBG_EN(1) | LPIT_MCR_DOZE_EN(1) | LPIT_MCR_M_CEN_MASK);

	// Setup the channel counters operation mode to "32-bit Periodic Counter",and keep default values for the trigger source;
	LPIT0->CHANNEL[0].TCTRL &= ~ LPIT_TCTRL_MODE_MASK;
	LPIT0->CHANNEL[0].TCTRL |= LPIT_TCTRL_MODE(0);			// set 00 to mode field

	// Set timer period for channel 0 as 1 second - reload
	LPIT0->CHANNEL[0].TVAL = 1000000U;

	// Enable channel0 interrupt;
	LPIT0->MIER |= LPIT_MIER_TIE0(1U);
	NVIC_EnableIRQ(LPIT0_IRQn);
}

// LPIT_Enable
void LPIT_StartTimer()
{
	// Starts the timer counting after all configuration;
	LPIT0->SETTEN = LPIT_SETTEN_SET_T_EN_0(1U);
}

// LPIT_Disable
void LPIT_StopTimer()
{
	LPIT0->CLRTEN = LPIT_CLRTEN_CLR_T_EN_0(1U);
}

// void LPIT0_IRQHandler(void){}
