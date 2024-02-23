#ifndef __ADC_MYAPP_H__
#define __ADC_MYAPP_H__

// library
#include <MKE16Z4.h>
#include "Queue.h"					// queue saves SC1A[RA]

// data type

// variable

// function prototype
void ADC_Init();
void ADC_Start();
void ADC_CalcTemp();
Queue_Type ADC_GetQueueTempVar();

#endif /*__ADC_MYAPP_H__*/
