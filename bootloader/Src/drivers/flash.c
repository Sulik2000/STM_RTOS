#include "drivers/flash.h"
#include <stm32f411xe.h>

void (*flash_error_callback)(enum Flash_Error error) = 0;

void Flash_Init() {
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB; // Unlock the flash memory

    FLASH->CR |= FLASH_CR_PSIZE_1; // Set programming size to 32 bits (32-bit word)
    FLASH->CR |= FLASH_CR_PG; // Enable programming mode

    FLASH->CR |= FLASH_CR_ERRIE; // Enable error interrupt
}

void Flash_Sector_Erase(enum Flash_Region region){
    while(FLASH->SR & FLASH_SR_BSY) {
        // Wait for any ongoing flash operation to complete
        __NOP();
    }
    FLASH->CR |= FLASH_CR_SER; // Enable sector erase
    FLASH->CR &= ~FLASH_CR_SNB; // Clear the sector number bits
    switch(region){
        case FLASH_REGION_SECTOR_0:
            FLASH->CR |= (0b0000 << FLASH_CR_SNB_Pos);
            break;
        case FLASH_REGION_SECTOR_1:
            FLASH->CR |= (0b0001 << FLASH_CR_SNB_Pos);
            break;
        case FLASH_REGION_SECTOR_2:
            FLASH->CR |= (0b0010 << FLASH_CR_SNB_Pos);
            break;
        case FLASH_REGION_SECTOR_3:
            FLASH->CR |= (0b0011 << FLASH_CR_SNB_Pos);
            break;
        case FLASH_REGION_SECTOR_4:
            FLASH->CR |= (0b0100 << FLASH_CR_SNB_Pos);
            break;
        case FLASH_REGION_SECTOR_5:
            FLASH->CR |= (0b0101 << FLASH_CR_SNB_Pos);
            break;
        case FLASH_REGION_SECTOR_6:
            FLASH->CR |= (0b0110 << FLASH_CR_SNB_Pos);
            break;
        case FLASH_REGION_SECTOR_7:
            FLASH->CR |= (0b0111 << FLASH_CR_SNB_Pos);
            break;
    }

    FLASH->CR |= FLASH_CR_STRT; // Start the erase operation
    while(FLASH->SR & FLASH_SR_BSY) {
        // Wait for the erase operation to complete
        __NOP();
    }
    FLASH->CR &= ~FLASH_CR_SER; // Disable sector erase
}

void Flash_Program(uint32_t address, uint32_t data){
    while(FLASH->SR & FLASH_SR_BSY) {
        // Wait for any ongoing flash operation to complete
        __NOP();
    }
    *(volatile uint32_t*)address = data; // Write the data to the specified address
    while(FLASH->SR & FLASH_SR_BSY) {
        // Wait for the programming operation to complete
        __NOP();
    }
}

void Flash_Interrupt_Handler(void){
    if(FLASH->SR & FLASH_SR_WRPERR){
        FLASH->SR &= ~FLASH_SR_WRPERR; // Clear the write protection error flag
        if(flash_error_callback){
            flash_error_callback(WRITE_PROTECTION_ERROR);
        }
    }

    if(FLASH->SR & (FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)){
        FLASH->SR &= ~(FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR); // Clear the programming error flags
        if(flash_error_callback){
            flash_error_callback(PROGRAMMING_ERROR);
        }
    }
}

void Flash_Set_Error_Callback(void (*callback)(enum Flash_Error error)){
    flash_error_callback = callback;
}