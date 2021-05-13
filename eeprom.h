#ifndef __EEPROM_H
#define __EEPROM_H
#include "stm32f4xx.h"
//#include "stm32f4xx_flash.h"
#define EEPROM_START_ADDRESS   ((uint32_t)0x08060000) // EEPROM emulation start address: after 112KByte of used Flash memory
 
FLASH_Status enableEEPROMWriting(void); // Unlock and keep PER cleared
void disableEEPROMWriting(void); // Lock
 
// Write functions
FLASH_Status writeEEPROMWord(uint32_t address, uint32_t data);
 
// Read functions
uint32_t readEEPROMWord(uint32_t address);
 
#endif
