#ifndef LOAD_FW_H
#define LOAD_FW_H

#include <stdint.h>

extern uint32_t _begin_app; // Symbol defined in the linker script, representing the start of the application code

void Load_FW_Send_Flash_Acknowledgment();
void Load_FW_Receive_Code_Chunks(const char* message, const uint16_t length);
void Load_FW_Flash_Start();

#endif // LOAD_FW_H