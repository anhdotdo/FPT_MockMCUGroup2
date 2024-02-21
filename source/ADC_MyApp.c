#include "ADC_MyApp.h"

// variable

// function definition
static void ADC_Start_INT(){			// => note: enable int
	// enable ADC complete interrupt
	ADC0->SC1[0] |= ADC_SC1_AIEN(1U);

	// NVIC
	NVIC_EnableIRQ(ADC0_IRQn);
}

void ADC_Init()
{
	// Init Clock - Async Clock
	PCC->CLKCFG[PCC_ADC0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;	// disable bus clock
	SCG->SIRCDIV &= ~SCG_SIRCDIV_SIRCDIV2_MASK;
	SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV2(4U);				// divide 8
	PCC->CLKCFG[PCC_ADC0_INDEX] &= ~PCC_CLKCFG_PCS_MASK;
	PCC->CLKCFG[PCC_ADC0_INDEX] |= PCC_CLKCFG_PCS(2U);		// Slow IRC clock

	// Bus Clock
	PCC->CLKCFG[PCC_ADC0_INDEX] |= PCC_CLKCFG_CGC(1U);

	// update system core clock
	SystemCoreClockUpdate();

	// CLock source /  Clock Divide
	ADC0->CFG1 &= ~(ADC_CFG1_ADICLK_MASK);					// ALTCLK1

	ADC0->CFG1 &= ~ADC_CFG1_ADIV_MASK;
	ADC0->CFG1 |= ADC_CFG1_ADIV(3U);						// divide 8

	// Resolution bit
	ADC0->CFG1 &= ~(ADC_CFG1_MODE_MASK);
	ADC0->CFG1 |= ADC_CFG1_MODE(2U);		// 10 bit conversion

	// Sample Time Clock
	ADC0->CFG2 = 255;

	// Voltage Reference
	ADC0->SC2 &= ~(ADC_SC2_REFSEL_MASK);

	// Choice Single Mode or Continous	=>note: fix sang single, khong dung stop adc
//	ADC0->SC3 |= ADC_SC3_ADCO(1U);				// continuous mode
	ADC0->SC3 &= ~ADC_SC3_ADCO_MASK;			// single mode

	// Trigger source
	ADC0->SC2 &= ~(ADC_SC2_ADTRG_MASK);			// software trigger

	// Interrupt? / polling?
	ADC_Start_INT();
}

static inline void ADC_Stop(){
	// SC1A[ADCH] = 0b11111 = 0x31
	ADC0->SC1[0] = (ADC0->SC1[0] & ~ADC_SC1_ADCH_MASK) | ADC_SC1_ADCH(0x1FU);
}

void ADC_Start()
{
	// start ADC
	ADC0->SC1[0] = ((ADC0->SC1[0]) & ~ADC_SC1_ADCH_MASK) | ADC_SC1_ADCH(0x1A);
}

static volatile uint16_t g_adc_result;
static volatile uint8_t g_adc_complete_flag = 0;
//static volatile uint16_t g_adc_counter;

void ADC0_IRQHandler(void)
{
	// read data
	g_adc_result = (uint16_t)(ADC0->R[0]);
	g_adc_complete_flag = 1;					// finished flag
//	g_adc_counter ++;
	// stop ADC conversion - due to single mode
}

// function calculate and return temp result
float ADC_GetTempResult()
{
	float Temp1;

	if(0U == g_adc_complete_flag)
	{
		return 0;
	}

	float Vtemp = g_adc_result * 0.003125 ; 			//Convert the ADC reading into voltage
	if(Vtemp >= .7012){								//Check for Hot or Cold Slope
		Temp1 = 25 - ((Vtemp - .7012)/ .001646);	//Cold Slope
	}else {
		Temp1 = 25 - ((Vtemp - .7012)/ .001749);	//Hot Slope
	}
	return Temp1;
}

// ---------------------------------------------------------------
//// queue saves 10 latest temperature value
//static Queue_Type queue_temperature;
//
//// queue save conversion results
//static Queue_Type queue_conv_result;
//
//// queue status
//static Queue_Status_Type queue_status;

//// function save conversion results
//void ADC_SaveConvResultToQueue()
//{
//	if(1U == g_adc_complete_flag)			// conversion finish
//	{
//		// clear flag
//		g_adc_complete_flag = 0;
//		// save to queue
////		queue_status = Queue_ForcePush(g_adc_result);
//		Queue_ForcePush(g_adc_result);
//	}
//}
//
//// function calculate temperature value

