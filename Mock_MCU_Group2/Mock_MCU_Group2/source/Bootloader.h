#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include <stdint.h>

#define ADDRESS_APP_CURRENT		((uint32_t)0x0000F000)
#define APP1_START_ADDRESS		((uint32_t)0x00008000)
#define APP2_START_ADDRESS		((uint32_t)0x0000C000)

typedef void (*pFunction_t)(void);
void Bootloader_JumptoApp(const uint32_t Address);

#endif /* BOOTLOADER_H_ */
