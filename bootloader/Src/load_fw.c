#include "load_fw.h"
#include "drivers/flash.h"
#include "log.h"
#include "string.h"

static uint32_t _code_counter = 0; // Counter to keep track of the number of code chunks received
static volatile uint8_t _bytes_collected_flag = 0; // Flag to indicate if bytes have been collected
static uint8_t _bytes[256];
static uint16_t bytes_length = 0;

void Load_FW_Update_Complete(){
    LOG_send(LOAD_FW_UPDATE_COMPLETE_MESSAGE, LOAD_FW_UPDATE_COMPLETE_MESSAGE_LENGTH);
    Flash_Deinit();
    _code_counter = 0; // Reset the code counter
}

void Load_FW_Send_Flash_Error(enum Flash_Error error){
    LOG_send(LOAD_FW_ERROR_MESSAGE, LOAD_FW_ERROR_MESSAGE_LENGTH);
}

void Load_FW_Collect_Bytes(const char* message, const uint16_t length){
    str_copy((char*)_bytes, message, length);
    _bytes_collected_flag = 1;
    bytes_length = length;
}

void Load_FW_Write_Code_Chunks(const char* message, const uint16_t length){
    if(length > 0){
        for(uint16_t i = 0; i < length; i += 4){
            uint32_t address = (uint32_t)&_begin_app + _code_counter;
            _code_counter += 4;
            Flash_Program(address, *(uint32_t*)(message + i));
        }
    }
}

void Load_FW_Send_Flash_Acknowledgment(){
    LOG_send(LOAD_FW_ACKNOWLEDGMENT_MESSAGE, LOAD_FW_ACKNOWLEDGMENT_MESSAGE_LENGTH);
}

void Load_FW_Receive_Code_Chunks(const char* message, const uint16_t length){
    if(length == LOAD_FW_UPDATE_COMPLETE_MESSAGE_LENGTH && str_cmp(message, LOAD_FW_UPDATE_COMPLETE_MESSAGE, LOAD_FW_UPDATE_COMPLETE_MESSAGE_LENGTH)){
        Load_FW_Update_Complete();
        return;
    }
    Load_FW_Write_Code_Chunks(message, length);
    Load_FW_Send_Flash_Acknowledgment();
}

void Load_FW_Flash_Start(){
    if(!LOG_Is_initialized())
        return; // Can't proceed if the log system is not initialized
    Flash_Init();
    Flash_Sector_Erase(FLASH_REGION_SECTOR_1); // Erase sector 1
    Flash_Sector_Erase(FLASH_REGION_SECTOR_2); // Erase sector 2
    Flash_Sector_Erase(FLASH_REGION_SECTOR_3); // Erase sector 3
    Flash_Sector_Erase(FLASH_REGION_SECTOR_4); // Erase sector 4
    Flash_Sector_Erase(FLASH_REGION_SECTOR_5); // Erase sector 5
    Flash_Sector_Erase(FLASH_REGION_SECTOR_6); // Erase sector 6
    Flash_Sector_Erase(FLASH_REGION_SECTOR_7); // Erase sector 7

    LOG_set_receive_callback(Load_FW_Collect_Bytes);
    Load_FW_Send_Flash_Acknowledgment();
    while(1){
        if(_bytes_collected_flag){
            Load_FW_Receive_Code_Chunks((const char*)_bytes, bytes_length);
            bytes_length = 0; // Reset the length after processing
            _bytes_collected_flag = 0; // Reset the flag after processing
        }
    }
}