#include "Port.h"

static PortCallback_t s_callback;

void PORT_Init(PORT_Type * PORTx, uint8_t pin, const PortConfigType * UserConfig)
{
	// Enable Clock for PORTD
	PCC->CLKCFG[PCC_PORTD_INDEX] |= PCC_CLKCFG_CGC(1U);

	// MUX
	PORTx -> PCR[pin] &= ~PORT_PCR_MUX_MASK;
	PORTx -> PCR[pin] |= PORT_PCR_MUX(UserConfig -> mux);

	// PULL
	PORTx -> PCR[pin] &= ~PORT_PCR_PE_MASK;
	PORTx -> PCR[pin] &= ~PORT_PCR_PS_MASK;
	PORTx -> PCR[pin] |= (UserConfig -> pull << PORT_PCR_PS_SHIFT);

	// Interrupt
	PORTx -> PCR[pin] &= ~PORT_PCR_IRQC_MASK;
	PORTx -> PCR[pin] |= PORT_PCR_IRQC(UserConfig -> interrupt);
	s_callback = UserConfig -> callback;
}

static inline void ClearInterruptFlag(PORT_Type * PORTx, uint8_t pin)
{
	PORTx -> ISFR = (1U << pin);
}

void PORTBCD_IRQHandler(void)
{
	uint8_t pin;

	for (pin = 0; pin < 32; pin++)
	{
		if (((PORTD -> ISFR) & (1U << pin)) != 0)
		{
			ClearInterruptFlag(PORTD, pin);
			// Application Code
			// User Code
			(*s_callback)(pin);
		}
	}

	NVIC_ClearPendingIRQ(PORTBCD_IRQn);
}

