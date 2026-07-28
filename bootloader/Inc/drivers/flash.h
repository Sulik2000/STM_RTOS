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

/**
 * Initializes the flash memory for writing. This function prepares the flash memory for programming.
 */
void Flash_Init();

/**
 * Erases the specified flash sector.
 */
void Flash_Sector_Erase(enum Flash_Region region);

/**
 * Programs a 32-bit word at the specified flash address.
 */
void Flash_Program(uint32_t address, uint32_t data);

/**
 * Handles flash interrupts.
 */
void Flash_Interrupt_Handler(void);
/**
 * Sets the callback function that will be called when a flash error occurs.
 */
void Flash_Set_Error_Callback(void (*callback)(enum Flash_Error error));

/**
 * Deinitializes the flash memory interface.
 */
void Flash_Deinit();

#endif // DRIVERS_FLASH_H