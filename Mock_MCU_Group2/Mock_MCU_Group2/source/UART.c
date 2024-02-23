
#include "UART.h"

void UART_ClockPort_Init()
{
	// Clock
	// Init Clock
	// Enable Clock for LPUART0
	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;

	SCG->FIRCDIV &= ~ SCG_FIRCDIV_FIRCDIV2_MASK;
	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(1);				// FIRC div 1 = 48MHz
	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~PCC_CLKCFG_PCS_MASK;
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_PCS(3U);		// Fast IRC Clock

	// Bus Clock
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_CGC(1U);

	// Port
	// Enable Clock for PORTB
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1U);

	// PB0 - UART0_RX - ALT2 - PULL UP
	PORTB->PCR[0] = (PORTB->PCR[0] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(2U);
	PORTB->PCR[0] |= 3U << PORT_PCR_PS_SHIFT;
	// PB1 - UART0_TX - ALT2 - PULL UP
	PORTB->PCR[1] = (PORTB->PCR[0] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(2U);
	PORTB->PCR[1] |= 3U << PORT_PCR_PS_SHIFT;
}

void UART_Init()
{
	UART_ClockPort_Init();

	// Baud rate
		// Asynch clock

		// Clear bit TE and RE
	LPUART0->CTRL &= ~ LPUART_CTRL_TE_MASK;
	LPUART0->CTRL &= ~ LPUART_CTRL_RE_MASK;

	// Baud rate = 9600
		// BAUD Divider
	LPUART0->BAUD = ( LPUART0->BAUD & ~LPUART_BAUD_SBR_MASK ) | LPUART_BAUD_SBR(625U);

		// OSR Divider
	LPUART0->BAUD |= LPUART_BAUD_BOTHEDGE(1U);
	LPUART0->BAUD = ( LPUART0->BAUD & ~LPUART_BAUD_OSR_MASK ) | LPUART_BAUD_OSR(7U);

	// Frame
		// Data length 8 bit   M10 -> M7? -> M
	LPUART0->BAUD &= ~LPUART_BAUD_M10_MASK;
	LPUART0->CTRL &= ~LPUART_CTRL_M7_MASK;
	LPUART0->CTRL &= ~LPUART_CTRL_M_MASK;

		// Parity disable
	LPUART0->CTRL &= ~LPUART_CTRL_PE_MASK;

		// Number stop bit : 1bit
	LPUART0->BAUD &= ~ LPUART_BAUD_SBNS_MASK;

		// LSB First or MSB First
	LPUART0->STAT &= ~LPUART_STAT_MSBF_MASK;
}

void UART_SendString(uint8_t * str, uint8_t len)
{
	uint8_t index = 0;

	// Enable Transmitter
	LPUART0->CTRL |= LPUART_CTRL_TE(1U);

	for (index = 0U; index < len; index++)
	{
		// Wait TX buffer empty
		while ((LPUART0->STAT & LPUART_STAT_TDRE_MASK) == 0);

		LPUART0->DATA = str[index];
	}

	while ((LPUART0->STAT & (1<<LPUART_STAT_TC_SHIFT)) == 0);

	LPUART0->CTRL &= ~LPUART_CTRL_TE_MASK;
}

void UART_StarReceiveINT()
{
	// Enable Receiver
	LPUART0->CTRL |= LPUART_CTRL_RE(1U);

	// Interrupt of Polling
		// Rx Interrupt
	LPUART0->CTRL |= LPUART_CTRL_RIE(1U);
		// NVIC
	NVIC_EnableIRQ(LPUART0_IRQn);
}

//-------------------Srec--------------

volatile static StepType step = SREC_DEFAULT;
volatile static SrecType Srec;
volatile static uint8_t addrcount = 0;
volatile static uint8_t datacount = 0;
volatile static uint8_t index = 0;
volatile static uint32_t check_checksum = 0;

void Srec_Reset()
{
	check_checksum = 0;
	Srec.Address = 0U;
	Srec.ByteCount = 0U;
	Srec.CheckSum = 0U;
}
//-------------------Srec--------------

//-------------------Command--------------

volatile RequestType Command;
volatile CommandType step_command = CMD_START;
volatile uint8_t requestcount = 0;
volatile uint8_t Times = 1;
volatile uint8_t option = 1;
volatile uint8_t idx = 0;
volatile static uint8_t erase = 0;
volatile static uint8_t load = 0;
volatile static uint8_t add = 0;

//-------------------Command--------------

volatile uint8_t Data = 0;

void LPUART0_IRQHandler(void)
{
	if ( 0U != (LPUART0->STAT & LPUART_STAT_RDRF_MASK) )
	{
		if (Times == 1U)
		{
			Data = (uint8_t)LPUART0->DATA;
			if ('5' == Data || 'S' == Data)
			{
				option = Data;
				Times = 2;
			}
		}
		else
		{
			switch (option)
			{
				case '5':
				{
					Data = (uint8_t)LPUART0->DATA;
					switch (step_command)
					{
						case CMD_START:
						{
							if ('+' == Data)
							{
								step_command = CMD_REQUEST;
							}
							break;
						}
						case CMD_REQUEST:
						{
							if ('+' == Data)
							{
								Command.Request[requestcount] = '\0';
								step_command = CMD_ADDRESS;
								requestcount = 0;
							}
							else
							{
								Command.Request[requestcount] = Data;
								requestcount++;
							}
							break;
						}
						case CMD_ADDRESS:
						{
							if ('+' == Data)
							{
								if ((strcmp("ERASE", Command.Request)) == 0)
								{
									step_command = CMD_NUM_OF_SECTOR;
									erase = 1;
								}
								else if ((strcmp("LOAD", Command.Request)) == 0)
								{
									load = 1;
									step_command = CMD_END1;
								}
								else if ((strcmp("ADD", Command.Request)) == 0)
								{
									add = 1;
									step_command = CMD_END1;
								}
								else
								{
									UART_SendString((uint8_t *)"\nCommand Error\n", 17);
									Times = 1;
								}

								addrcount = 0;
							}
							else
							{
								addrcount++;
								Command.Address += ChartoHex(Data) * pow(16, 4 - addrcount);
							}
							break;
						}
						case CMD_NUM_OF_SECTOR:
						{
							if ('+' == Data)
							{
								step_command = CMD_END1;
							}
							else
							{
								Command.NumOfSector = ChartoHex(Data);
							}
							break;
						}
						case CMD_END1:
						{
							if ('A' == Data)
							{
								step_command = CMD_END2;
							}
							else
							{
								UART_SendString((uint8_t *)"\nCommand Error\n", 17);
								Times = 1;
							}
							break;
						}
						case CMD_END2:
						{
							if ('A' == Data)
							{
								if (1U == erase)
								{
									if ((*((uint32_t *)(Command.Address))) == 0x20001800U)
									{
										Flash_Erase_MultiSector(Command.Address, Command.NumOfSector);
										UART_SendString((uint8_t *)"\nApp deleted\n", 15);
									}
									else
									{
										UART_SendString((uint8_t *)"\nApp does not exist, can not delete\n", 38);
									}

									if (*((uint32_t *)(APP1_START_ADDRESS)) == 0x20001800)
									{
										Flash_Erase_Sector(ADDRESS_APP_CURRENT);
										Flash_Program_Word(ADDRESS_APP_CURRENT, APP1_START_ADDRESS);
									}
									else if (*((uint32_t *)(APP2_START_ADDRESS)) == 0x20001800)
									{
										Flash_Erase_Sector(ADDRESS_APP_CURRENT);
										Flash_Program_Word(ADDRESS_APP_CURRENT, APP2_START_ADDRESS);
									}
									else
									{
										Flash_Erase_Sector(ADDRESS_APP_CURRENT);
									}
									erase = 0;
									MenuOption();
								}
								if (1U == load)
								{
									if ((*((uint32_t *)(Command.Address))) == 0x20001800U)
									{
										UART_SendString((uint8_t *)"\nApp selected, please reset board\n", 36);
										if (APP1_START_ADDRESS == Command.Address)
										{
											Flash_Erase_Sector(ADDRESS_APP_CURRENT);
											Flash_Program_Word(ADDRESS_APP_CURRENT, APP1_START_ADDRESS);
										}
										else if (APP2_START_ADDRESS == Command.Address)
										{
											Flash_Erase_Sector(ADDRESS_APP_CURRENT);
											Flash_Program_Word(ADDRESS_APP_CURRENT, APP2_START_ADDRESS);
										}
									}
									else
									{
										UART_SendString((uint8_t *)"\nApp does not exist, please send the file first\n", 50);
										MenuOption();
									}
									load = 0;
								}
								if (1U == add)
								{
									UART_SendString((uint8_t *)"\nSend File, please!\n", 22);
									Flash_Erase_MultiSector(Command.Address, 8);
									Flash_Erase_Sector(ADDRESS_APP_CURRENT);
									Flash_Program_Word(ADDRESS_APP_CURRENT, Command.Address);
									add = 0;
								}
							}
							else
							{
								UART_SendString((uint8_t *)"\nCommand Error\n", 17);
							}
							step_command = CMD_START;
							Times = 1;

							for (idx = 0; idx < 6; idx ++)
							{
								Command.Request[idx] = 0;
							}
							Command.Address = 0;
							break;
						}
					}
				}

				case 'S':
				{
					Data = (uint8_t)LPUART0->DATA;

					switch (step)
					{
						case SREC_DEFAULT:
						{
							if ( 'S' == Data )
							{
								step = SREC_S_CHAR;
							}
							break;
						}
						case SREC_S_CHAR:
						{
							Srec.Stype = ChartoHex(Data);
							switch (Srec.Stype)
							{
								case 0U:
								{
									step = SREC_DEFAULT;
									break;
								}
								case 1U:
								case 2U:
								case 3U:
								{
									Srec.NumOfAddress = 2 * (Srec.Stype + 1);
									step = SREC_S_TYPE;
									break;
								}
								case 7U:
								case 8U:
								case 9U:
								{
									UART_SendString((uint8_t *)"\nFile OK! Please Reset Board to launch APP\n", 45);

									break;
								}
							}
							break;
						}
						case SREC_S_TYPE:
						{
							Srec.ByteCount += ChartoHex(Data) * 16;
							step = SREC_BYTECOUNT_0;
							break;
						}
						case SREC_BYTECOUNT_0:
						{
							Srec.ByteCount += ChartoHex(Data);
							step = SREC_BYTECOUNT_1;
							check_checksum += Srec.ByteCount;
							Srec.NumOfData = Srec.ByteCount * 2 - Srec.NumOfAddress - 2;
							break;
						}
						case SREC_BYTECOUNT_1:
						case SREC_ADDRESS:
						{
							addrcount++;
							Srec.Address += ChartoHex(Data) * pow(16, Srec.NumOfAddress - addrcount);

							if (addrcount % 2 == 0)
							{
								check_checksum += (uint8_t)(Srec.Address >> (4 * (Srec.NumOfAddress - addrcount)));
							}

							if (addrcount == Srec.NumOfAddress)
							{
								step = SREC_DATAWORD;
								addrcount = 0;
							}
							else
							{
								step = SREC_ADDRESS;
							}
							break;
						}
						case SREC_DATAWORD:
						{
							datacount++;
							index++;

							if ((datacount % 2) == 0)
							{
								Srec.DataWord |= ChartoHex(Data) << ((index - 2) * 4);
								check_checksum += (uint8_t)(Srec.DataWord >> (4 * (index - 2)));
							}
							else
							{
								Srec.DataWord |= ChartoHex(Data) << (index * 4);
							}

							if ((datacount % 8) == 0)
							{
								Flash_Program_Word(Srec.Address, Srec.DataWord);
								Srec.Address += 4;
								Srec.DataWord = 0;
								index = 0;
							}

							if ( datacount == Srec.NumOfData )
							{
								if ((datacount % 8) != 0)
								{
									Flash_Program_Word(Srec.Address, Srec.DataWord);
									Srec.Address += 4;
								}
								step = SREC_CHECKSUM_1;
								Srec.DataWord = 0;
								datacount = 0;
								index = 0;
							}
							else
							{
								step = SREC_DATAWORD;
							}
							break;
						}
						case SREC_CHECKSUM_1:
						{
							Srec.CheckSum += ChartoHex(Data) * 16;
							step = SREC_CHECKSUM_2;
							break;
						}
						case SREC_CHECKSUM_2:
						{
							Srec.CheckSum += ChartoHex(Data);

							// Cal check sum and check
							check_checksum += Srec.CheckSum;

							if (0xFFU != ((uint8_t)(check_checksum & 0xFFU)))
							{
								UART_SendString((uint8_t *)"\nFile Error\n", 14);
								Flash_Erase_MultiSector(0xA000, 4);
							}

							step = SREC_DEFAULT;
							check_checksum = 0;
							Srec_Reset();
							break;
						}
					}
				}
			}
		}
	}
}

uint8_t ChartoHex(uint8_t character)
{
	uint8_t result;

	if ( character <= '9' && character >= '0' )
	{
		result = character - '0';
	}
	else if ( character <= 'F' && character >= 'A' )
	{
		result = character - 'A' + 0xA;
	}
	else
	{
		result = 0xFF;
	}

	return result;
}

void MenuOption()
{
	UART_SendString((uint8_t *)"\n---------------- Boot Mode ----------------\n", 47);
	UART_SendString((uint8_t *)"\n1.Add Application: 55+ADD+Address+AA\n", 40);
	UART_SendString((uint8_t *)"\n2.Erase Application: 55+ERASE+Address+n+AA\n", 46);
	UART_SendString((uint8_t *)"\n3.Load Application: 55+LOAD+Address+AA\n", 42);
	UART_SendString((uint8_t *)"\n------------------- END -------------------\n", 47);
}
