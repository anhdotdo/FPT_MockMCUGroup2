#include <MKE16Z4.h>
#include "UART_MyApp.h"
#include "ADC_MyApp.h"
#include "LPIT_MyApp.h"
#include "Queue.h"					// use Queue_Init()

void LPIT0_IRQHandler(void)
{
	// clear interrupt flag
	LPIT0->MSR |= LPIT_MSR_TIF0(1U);

	// app code
	ADC_Start();	// initialize a new conversion
}

int main(void) {

	LPIT_Init();
	ADC_Init();
	UART_INIT();
	Queue_Init();

	LPIT_StartTimer();
	LPUART0_StartReceiveINT();

    while(1) {
    	if(1U == UART_GetRxIntFlagVar())		// receive interrupt flag
    	{
    		// respond command
    		LPUART_WriteTempToPC();

    		// clear flag
    		UART_ClearRxIntFlagVar();
    	}
    }
    return 0 ;
}
