#include <MKE16Z4.h>
#include "UART_MyApp.h"
#include "ADC_MyApp.h"
#include "LPIT_MyApp.h"
//#include "Queue.h"

void LPIT0_IRQHandler(void)
{
	// clear interrupt flag
	LPIT0->MSR |= LPIT_MSR_TIF0(1U);

	// app code
	ADC_Start();	// initialize a new conversion
}

int main(void) {
	// variable
//	float temp_result;

//	UART_Init();
//	UART_TransmitByte('A');

//	ADC_Init();
//	ADC_Start();
//	adc_result = ADC_GetG_ADC_Result();
//	temp_result = ADC_CalcTempValue(adc_result);

	LPIT_Init();
	ADC_Init();
	LPIT_StartTimer();

    while(1) {
    	// get temp result
//    	temp_result = ADC_GetTempResult();
    }
    return 0 ;
}
