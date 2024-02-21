#ifndef __UART_MYAPP_H__
#define __UART_MYAPP_H__

// library
#include <MKE16Z4.h>
#include "LPIT_MyApp.h"				// use LPIT_StartTimer, LPIT_StopTimer funcs
#include "Queue.h"					// use result saved in queue
#include "ADC_MyApp.h"				// use ADC_CalcTemp, ADC_GetQueueTempVar funcs

// variable

// data type

// function prototype
void UART_INIT();
void UART_SendByte(uint8_t ch);
void UART_SendString(uint8_t* str, uint8_t len);
void LPUART0_StartReceiveINT();
void LPUART_WriteTempToPC();
uint8_t UART_GetRxIntFlagVar();
void UART_ClearRxIntFlagVar();

#endif /*__UART_MYAPP_H__*/
