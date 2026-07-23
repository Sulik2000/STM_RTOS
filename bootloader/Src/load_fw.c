#include "load_fw.h"
#include "drivers/flash.h"
#include "log.h"
#include "string.h"
#include "stm32f4xx.h"

#define LOAD_FW_ACKNOWLEDGMENT_MESSAGE ((char[]){0xDD, 0xDD, '\0'})
#define LOAD_FW_ERROR_MESSAGE ((char[]){0xDD, 0x01, '\0'})
#define LOAD_FW_UPDATE_COMPLETE_MESSAGE ((char[]){0xFF, 0xFF, 0xDD, 0xDD, '\0'}) // Value which sent by the host to indicate that the firmware update is complete

void Load_FW_Update_Complete(){
    log_message("Firmware update complete. Waiting on reset...\n");
}

void Load_FW_Send_Flash_Error(enum Flash_Error error){
    log_message(LOAD_FW_ERROR_MESSAGE);
}

void Load_FW_Write_Code_Chunks(const char* message, const uint16_t length){
    if(length > 0){
        for(uint16_t i = 0; i < length; i += 4){
            uint32_t address = (uint32_t)&_begin_app + i;
            Flash_Program(address, *(uint32_t*)(message + i));
        }
        Load_FW_Send_Flash_Acknowledgment();
    }
    else{
        Load_FW_Send_Flash_Error(PROGRAMMING_ERROR);
    }
}

void Load_FW_Send_Flash_Acknowledgment(){
    log_message(LOAD_FW_ACKNOWLEDGMENT_MESSAGE);
}

void Load_FW_Receive_Code_Chunks(const char* message, const uint16_t length){
    if(length == str_length(LOAD_FW_UPDATE_COMPLETE_MESSAGE) && str_cmp(message, LOAD_FW_UPDATE_COMPLETE_MESSAGE, str_length(LOAD_FW_UPDATE_COMPLETE_MESSAGE))){
        Load_FW_Update_Complete();
        return;
    }
    Load_FW_Write_Code_Chunks(message, length);
    Load_FW_Send_Flash_Acknowledgment();
}

void Load_FW_Flash_Start(){
    if(!is_log_system_initialized())
        return; // Can't proceed if the log system is not initialized
    Flash_Init();
    Flash_Sector_Erase(FLASH_REGION_SECTOR_1); // Erase sector 1
    Flash_Sector_Erase(FLASH_REGION_SECTOR_2); // Erase sector 2
    Flash_Sector_Erase(FLASH_REGION_SECTOR_3); // Erase sector
    Flash_Sector_Erase(FLASH_REGION_SECTOR_4); // Erase sector 4
    Flash_Sector_Erase(FLASH_REGION_SECTOR_5); // Erase sector 5
    Flash_Sector_Erase(FLASH_REGION_SECTOR_6); // Erase sector 6
    Flash_Sector_Erase(FLASH_REGION_SECTOR_7); // Erase sector 7

    set_log_receive_callback(Load_FW_Receive_Code_Chunks);
    Load_FW_Send_Flash_Acknowledgment();
    while(1){}
}