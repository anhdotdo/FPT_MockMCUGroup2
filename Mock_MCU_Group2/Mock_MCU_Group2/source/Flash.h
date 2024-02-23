#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>

#define CMD_PROGRAM_LONGWORD	0x06
#define CMD_ERASE_FLASH_SECTOR	0x09
#define CMD_ERASE_ALL_BLOCKS	0x44

void Flash_Program_Word(uint32_t Address, uint32_t Data);
void Flash_Erase_Sector(uint32_t Address);
void Flash_Erase_All_Sectors();
void Flash_Erase_MultiSector(uint32_t Address, uint8_t NumOfSector);

#endif /* FLASH_H_ */
