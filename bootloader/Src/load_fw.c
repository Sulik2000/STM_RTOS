#include "load_fw.h"
#include "drivers/flash.h"
#include "log.h"

void Load_FW_Send_Flash_Acknowledgment(){
    log_message((char[]){0xDD, 0xDD, '\0'});
}

void Load_FW_Receive_Code_Chunks(const char* message, const uint16_t length){
    Load_FW_Send_Flash_Acknowledgment();
}

void Load_FW_Flash_Start(){
    if(!is_log_system_initialized())
        return;

    set_log_receive_callback(Load_FW_Receive_Code_Chunks);
    Load_FW_Send_Flash_Acknowledgment();
    while(1){}
}