#ifndef __UART_MYAPP_H__
#define __UART_MYAPP_H__

// library
#include <MKE16Z4.h>

// variable

// data type

// function prototype
void UART_Init();
void UART_TransmitByte(uint8_t ch);
void UART_TransmitString(uint8_t * str, uint8_t len);

#endif /*__UART_MYAPP_H__*/
