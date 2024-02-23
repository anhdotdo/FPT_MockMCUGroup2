
#ifndef _UART_H_
#define _UART_H_

#include "MKE16Z4.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "Flash.h"
#include "Bootloader.h"

typedef struct
{
	uint8_t Stype;
	uint8_t ByteCount;
	uint32_t Address;
	uint8_t NumOfAddress;
	uint32_t DataWord;
	uint8_t NumOfData;
	uint8_t CheckSum;
} SrecType;

typedef enum
{
	SREC_DEFAULT,
	SREC_S_CHAR,
	SREC_S_TYPE,
	SREC_BYTECOUNT_0,
	SREC_BYTECOUNT_1,
	SREC_ADDRESS,
	SREC_DATAWORD,
	SREC_CHECKSUM_1,
	SREC_CHECKSUM_2
} StepType;

/************ Part Command *************/

typedef struct
{
	char Request[6];
	uint32_t Address;
	uint8_t NumOfSector;
} RequestType;

typedef enum
{
	CMD_START,
	CMD_REQUEST,
	CMD_ADDRESS,
	CMD_NUM_OF_SECTOR,
	CMD_END1,
	CMD_END2
} CommandType;

/************ Part Command End *************/

uint8_t ChartoHex(uint8_t character);
void Srec_Reset();
void UART_StarReceiveINT();
void UART_SendString(uint8_t * str, uint8_t len);
void UART_Init();
void UART_ClockPort_Init();
void MenuOption();

#endif /* _UART_H_ */
