#ifndef DRIVERS_FLASH_H
#define DRIVERS_FLASH_H

#include <stdint.h>

#ifdef STM32F4xx // Structure of Flash for STM32F4xx series
enum Flash_Region {
    FLASH_REGION_SECTOR_0 = 0,
    FLASH_REGION_SECTOR_1 = 1,
    FLASH_REGION_SECTOR_2 = 2,
    FLASH_REGION_SECTOR_3 = 3,
    FLASH_REGION_SECTOR_4 = 4,
    FLASH_REGION_SECTOR_5 = 5,
    FLASH_REGION_SECTOR_6 = 6,
    FLASH_REGION_SECTOR_7 = 7,
    FLASH_REGION_SYSTEM_MEMORY = 8,
    FLASH_REGION_OTP = 9,
    FLASH_REGION_OPTION_BYTES = 10
};
#endif

enum Flash_Error {
    WRITE_PROTECTION_ERROR = 1,
    PROGRAMMING_ERROR = 2,
};

void Flash_Init();
void Flash_Sector_Erase(enum Flash_Region region);
void Flash_Program(uint32_t address, uint32_t data);
void Flash_Interrupt_Handler(void);
void Flash_Set_Error_Callback(void (*callback)(enum Flash_Error error));

#endif // DRIVERS_FLASH_H