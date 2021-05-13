#include "eeprom.h"

/*
 * Must call this first to enable writing
 */
FLASH_Status enableEEPROMWriting() {
		FLASH_Unlock();

		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
		return FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3);
}
 
void disableEEPROMWriting() {
    FLASH_Lock();
}
 
/*
 * Writing functions
 * Must call enableEEPROMWriting() first
 */

FLASH_Status writeEEPROMWord(uint32_t address, uint32_t data) {

    address = (address*4) + EEPROM_START_ADDRESS;
    return FLASH_ProgramWord(address, data);
}
 
/*
 * Reading functions
 */
 
uint32_t readEEPROMWord(uint32_t address) {
    uint32_t val = 0;
    address = (address*4) + EEPROM_START_ADDRESS;
    val = *(__IO uint32_t*)address;
	return val;
}
