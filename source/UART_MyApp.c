#include "UART_MyApp.h"

// variable

// function definition
static void UART_ClockPort_Init()
{
	// clock
	// --asyn clock source
	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;		// disable bus clock

	SCG->SIRCDIV &= ~SCG_SIRCDIV_SIRCDIV2_MASK;
	SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV2(3U);					// divide 4

	SCG->SIRCCFG |= SCG_SIRCCFG_RANGE_MASK;						// 8MHZ

	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~ PCC_CLKCFG_PCS_MASK;
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_PCS(2U);		// SIRC

	// --bus clock for portB, LPUART0
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1U);
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_CGC(1U);

	// update system core clock
	SystemCoreClockUpdate();

	// port - TX : B1, RX : B0
	PORTB->PCR[1] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[1] |= PORT_PCR_MUX(2U);		// ALT2
	PORTB->PCR[1] |= PORT_PCR_PE(1U);
	PORTB->PCR[1] |= PORT_PCR_PS(1U);		// pull-up

	PORTB->PCR[0] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[0] |= PORT_PCR_MUX(2U);		// ALT2
	PORTB->PCR[0] |= PORT_PCR_PE(1U);
	PORTB->PCR[0] |= PORT_PCR_PS(1U);		// pull-up
}

void UART_INIT()
{
	 UART_ClockPort_Init();

	// CTRL[RE], CLTR[TE] are both = 0
	LPUART0->CTRL &= ~LPUART_CTRL_RE_MASK;
	LPUART0->CTRL &= ~LPUART_CTRL_TE_MASK;

	// baudrate 2M-> SBR: 52, OSR: 3
	// --baud divider
	LPUART0->BAUD = (LPUART0->BAUD & ~LPUART_BAUD_SBR_MASK) | LPUART_BAUD_SBR(52U);
	// --OSR DIVIDER
	LPUART0->BAUD |= LPUART_BAUD_BOTHEDGE(1U);
	LPUART0->BAUD = (LPUART0->BAUD & ~LPUART_BAUD_OSR_MASK) | LPUART_BAUD_OSR(3U);		// read-modify-write
	/*UART0 Tx B1
	 *UART0 Rx B0 */

	// frame
	// --data length (7/8/9/10) - M10 / M7 / M
	LPUART0->BAUD &= ~LPUART_BAUD_M10_MASK;
	LPUART0->CTRL &= ~LPUART_CTRL_M7_MASK;
	LPUART0->CTRL &= ~LPUART_CTRL_M_MASK;

	// --parity (even/odd/none) - PE / PT
	LPUART0->CTRL &= ~ LPUART_CTRL_PE_MASK;

	// --number of stop bits (1/2) - SBNS
	LPUART0->BAUD &= ~LPUART_BAUD_SBNS_MASK;

	// --lsb first or msb first - MSBF
	LPUART0->STAT &= ~LPUART_STAT_MSBF_MASK;
}

void UART_SendByte(uint8_t ch)
{
	// enable TE
	LPUART0->CTRL |= LPUART_CTRL_TE_MASK;

	// wait tx buffer is empty - TDRE
	while( 0U == (LPUART0->STAT & LPUART_STAT_TDRE_MASK));

	// TX_buffer = ch
	LPUART0->DATA = ch;

	// wait transmission is complete - TC
	while( 0U == (LPUART0->STAT & LPUART_STAT_TC_MASK));

	// disable TE
	LPUART0->CTRL &= ~LPUART_CTRL_TE_MASK;

}

void UART_SendString(uint8_t* str, uint8_t len)
{
	uint8_t idx;

	// enable TE
	LPUART0->CTRL |= LPUART_CTRL_TE_MASK;

	for(idx = 0; idx < len; idx++)
	{
		// wait TX buffer is empty - TDRE
		while( 0U == (LPUART0->STAT & LPUART_STAT_TDRE_MASK));

		// TX buffer = ch
		LPUART0->DATA = str[idx];
	}
	// wait transmission is complete - TC
	while( 0U == (LPUART0->STAT & LPUART_STAT_TC_MASK));

	// disable TE
	LPUART0->CTRL &= ~LPUART_CTRL_TE_MASK;
}

volatile uint8_t g_Data;
volatile uint8_t g_CorrCommandFlag = 0U;
volatile uint8_t RxIntFlag = 0U;

// function get RxIntFlag
uint8_t UART_GetRxIntFlagVar()
{
	return RxIntFlag;
}

// function clear RxIntFlag
void UART_ClearRxIntFlagVar()
{
	RxIntFlag = 0U;
}

//
void LPUART0_IRQHandler(void)
{
	if(0U != (LPUART0->STAT & LPUART_STAT_RDRF_MASK))
	{
		g_Data = LPUART0->DATA;
		RxIntFlag = 1U;

		if(0x55 == g_Data){
			g_CorrCommandFlag = 1U;

			// Stop LPIT
			LPIT_StopTimer();
		}
		else
		{
			g_CorrCommandFlag = 0U;
		}
	}
}

// function solve received command 0x55
#define MAX_SIZE 10								// size of queue
static Queue_Type queue_temp;

void LPUART_WriteTempToPC()
{
	uint8_t idx;
	uint32_t temperature;
	uint8_t x100;
	uint8_t x10;

	if(1U == g_CorrCommandFlag)
	{
		// stop Timer
//		LPIT_StopTimer();

		// write 10 latest temp value
		ADC_CalcTemp();					// calc from conversion result to temp value
		queue_temp = ADC_GetQueueTempVar();
		for(idx = 0; idx < MAX_SIZE; idx++)
		{
			temperature = queue_temp.QueueData[idx];
			// write x100
			x100 = temperature / 100;
			if(0U != x100)
			{
				UART_SendByte(0x30 + x100);
			}
			temperature %= 100;
			// write x10
			x10 = temperature/10;
			if(0U != x10)
			{
			UART_SendByte(0x30 + x10);
			}
			temperature %= 10;
			// write x1
			UART_SendByte(0x30 + temperature % 10);

			// end 1 value
			UART_SendString((uint8_t*)"  ", 2);
		}

		// end 1 correct command
		UART_SendByte('\n');

		// start Timer
		LPIT_StartTimer();

		// clear flag
		g_CorrCommandFlag = 0U;
	}
}

// enable RE
void LPUART0_StartReceiveINT()
{
	// interrupt or polling
		// Tx interrupt: tie / tcie
		// Rx interrupt: RIE
		// NVIC
	LPUART0->CTRL |= LPUART_CTRL_RIE(1U);
	NVIC_EnableIRQ(LPUART0_IRQn);

	// enable receive interrupt
	LPUART0->CTRL |= LPUART_CTRL_RE_MASK;
}
