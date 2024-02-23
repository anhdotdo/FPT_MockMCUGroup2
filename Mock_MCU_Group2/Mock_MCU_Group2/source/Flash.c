#include "Flash.h"
#include "MKE16Z4.h"

void Flash_Program_Word(uint32_t Address, uint32_t Data)
{
//	__disable_irq();

	// Wait until previous command complete
	// Check CCIF
	while (0 == (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK));

	// Clear the old errors
	// Check ACCERR & FPVIOL
	if (	(FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0
		||	(FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) != 0 )
	{
		// Write 0x30 to FSTAT register to clear the old errors
		FTFA->FSTAT = 0x30;
	}

	// Write Command to FCCOB0 register
	FTFA->FCCOB0 = CMD_PROGRAM_LONGWORD;

	// Write Parameters to FCCOBn registers
	// Fill Address
	FTFA->FCCOB1 = (uint8_t)(Address >> 16);
	FTFA->FCCOB2 = (uint8_t)(Address >> 8);
	FTFA->FCCOB3 = (uint8_t)(Address >> 0);

	// Fill Data
	FTFA->FCCOB4 = (uint8_t)(Data >> 24);
	FTFA->FCCOB5 = (uint8_t)(Data >> 16);
	FTFA->FCCOB6 = (uint8_t)(Data >> 8);
	FTFA->FCCOB7 = (uint8_t)(Data >> 0);

	// Clear the CCIF to launch the command
	// Write 0x80 to FSTAT register
	FTFA->FSTAT = 0x80;

	// Wait until this command complete
	// Check CCIF
	while (0 == (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK));

//	__enable_irq();
}

void Flash_Erase_Sector(uint32_t Address)
{
//	__disable_irq();

	// Wait until previous command complete
	// Check CCIF
	while (0 == (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK));

	// Clear the old errors
	// Check ACCERR & FPVIOL
	if (	(FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0
		||	(FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) != 0 )
	{
		// Write 0x30 to FSTAT register to clear the old errors
		FTFA->FSTAT = 0x30;
	}

	// Write Command to FCCOB0 register
	FTFA->FCCOB0 = CMD_ERASE_FLASH_SECTOR;

	// Write Parameters to FCCOBn registers
	// Fill Address
	FTFA->FCCOB1 = (uint8_t)(Address >> 16);
	FTFA->FCCOB2 = (uint8_t)(Address >> 8);
	FTFA->FCCOB3 = (uint8_t)(Address >> 0);

	// Clear the CCIF to launch the command
	// Write 0x80 to FSTAT register
	FTFA->FSTAT = 0x80;

	// Wait until this command complete
	// Check CCIF
	while (0 == (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK));

//	__enable_irq();
}

void Flash_Erase_MultiSector(uint32_t Address, uint8_t NumOfSector)
{
	uint8_t idx = 0;
	for (idx = 0; idx < NumOfSector; idx++)
	{
		Flash_Erase_Sector(Address + 0x400*idx);
	}
}

void Flash_Erase_All_Sectors()
{
	__disable_irq();

	// Wait until previous command complete
	// Check CCIF
	while (0 == (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK));

	// Clear the old errors
	// Check ACCERR & FPVIOL
	if (	(FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0
		||	(FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) != 0 )
	{
		// Write 0x30 to FSTAT register to clear the old errors
		FTFA->FSTAT = 0x30;
	}

	// Write Command to FCCOB0 register
	FTFA->FCCOB0 = CMD_ERASE_ALL_BLOCKS;

	// Clear the CCIF to launch the command
	// Write 0x80 to FSTAT register
	FTFA->FSTAT = 0x80;

	// Wait until this command complete
	// Check CCIF
	while (0 == (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK));

	__enable_irq();
}
