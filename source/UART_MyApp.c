#include "UART_MyApp.h"

// variable

// function definition
void UART_Init()
{
	// clock for UART - asyn peripheral clock & bus clock
	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;		// disable bus clock
	SCG->SIRCDIV &= ~SCG_SIRCDIV_SIRCDIV2_MASK;
	SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV2(3U);					// divide 4 -> f = 2Mhz

	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~PCC_CLKCFG_PCS_MASK;
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_PCS(2U);		// slow IRC, default clock = 8MHz

	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_CGC(1U);		// bus clock

	// clock for PORT - bus clock
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1U);

	// update system core clock
	SystemCoreClockUpdate();

	// Port
	PORTB->PCR[1] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[1] |= PORT_PCR_MUX(2U);				// mux: ALT2

	PORTB->PCR[1] |= PORT_PCR_PE(1U);
	PORTB->PCR[1] |= PORT_PCR_PS(1U);				// pull-up

	PORTB->PCR[0] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[0] |= PORT_PCR_MUX(2U);				// mux: ALT2

	PORTB->PCR[0] |= PORT_PCR_PE(1U);
	PORTB->PCR[0] |= PORT_PCR_PS(1U);				// pull-up

	// CTRL[RE], CTRL[TE] both = 0
	LPUART0->CTRL &= ~LPUART_CTRL_TE_MASK;
	LPUART0->CTRL &= ~LPUART_CTRL_RE_MASK;

	// calculate - f = 2MHz, baurade = 9600, (OSR+1) * SBR * baudrate = f
	// -- OSR divider: limit of 3 -> 31, = 3
	LPUART0->BAUD |= LPUART_BAUD_BOTHEDGE(1U);	// bothedge to be set
	LPUART0->BAUD = (LPUART0->BAUD & ~LPUART_BAUD_OSR_MASK) | LPUART_BAUD_OSR(3U);

	// -- baudrate divider - SBR: limit of 13 bit length, = 52
	LPUART0->BAUD = (LPUART0->BAUD & ~LPUART_BAUD_SBR_MASK) | LPUART_BAUD_SBR(52U);

	// fame
	// -- data length - M10, M7, M -> default 8 bit

	// -- stop bit
	LPUART0->BAUD &= ~LPUART_BAUD_SBNS_MASK;

	// -- parity
	LPUART0->CTRL &= ~LPUART_CTRL_PE_MASK;

	// -- MSB first
	LPUART0->STAT &= ~LPUART_STAT_MSBF_MASK;

	// polling or interrupt
		// Tx interrupt - tie, tcie
		// Rx interrupt - rie
		// NVCI
}

void UART_TransmitByte(uint8_t ch)
{
	// enable CTRL[TE]
	LPUART0->CTRL |= LPUART_CTRL_TE(1U);

	// wait for Tx buffer empty - TDRE = 1
	while(0U == (LPUART0->STAT & LPUART_STAT_TDRE_MASK));
	// assign to Tx_buffer - DATA
	LPUART0->DATA = ch;

	// wait for transmission complete - TC = 1
	while(0U == (LPUART0->STAT & LPUART_STAT_TC_MASK));

	// disable CTR[TE]
	LPUART0->CTRL &= ~LPUART_CTRL_TE_MASK;
}

void UART_TransmitString(uint8_t * str, uint8_t len)
{
	uint8_t idx;
	// enable CTRL[TE]
	LPUART0->CTRL |= LPUART_CTRL_TE(1U);

	// transmit string
	for(idx = 0; idx < len; idx++)
	{
		while(0U == (LPUART0->STAT & LPUART_STAT_TDRE_MASK));
		LPUART0->DATA = str[idx];
	}

	// wait until TC = 1
	while(0U == (LPUART0->STAT & LPUART_STAT_TC_MASK));

	// disable CTR[TE]
	LPUART0->CTRL &= ~LPUART_CTRL_TE_MASK;
}
