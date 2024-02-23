
#ifndef _PORT_H_
#define _PORT_H_

#include "MKE16Z4.h"
#include <stdint.h>

// Library(Driver)
// Function - PORT - Pin?
// Multiplexing / Pull / External Interrupt

// Example - PTD2 = Interrupt, falling edge
// PORTD / pin = 2 / MUX = GPIO / Pull Up / Falling edge

typedef enum {
	PORT_MUX_ANALOG		= 0U,
	PORT_MUX_GPIO		= 1U,
	PORT_MUX_ALT2		= 2U,
	PORT_MUX_ALT3		= 3U,
	PORT_MUX_ALT4		= 4U,
	PORT_MUX_ALT5		= 5U,
	PORT_MUX_ALT6		= 6U,
	PORT_MUX_ALT7		= 7U
} PortMUXType;

typedef enum {
	PORT_PULL_DISABLE	= 0U,
	PORT_PULL_DOWN		= 2U,
	PORT_PULL_UP		= 3U
} PortPULLType;

typedef enum {
	PORT_INTERRUPT_DISABLE		= 0U,
	PORT_INTERRUPT_LOW_LEVEL	= 8U,
	PORT_INTERRUPT_RASING_EDGE	= 9U,
	PORT_INTERRUPT_FALLING_EDGE	= 10U,
	PORT_INTERRUPT_BOTH_EDGE	= 11U,
	PORT_INTERRUPT_HIGH_LEVEL	= 12U,
} PortINTType;

typedef void (*PortCallback_t) (uint8_t);

typedef struct {
	PortMUXType mux;
	PortPULLType pull;
	PortINTType interrupt;
	PortCallback_t callback;
} PortConfigType;

void PORT_Init(PORT_Type * PORTx, uint8_t pin, const PortConfigType * UserConfig);

#endif /* _PORT_H_ */
