#ifndef LOAD_FW_H
#define LOAD_FW_H

#include <stdint.h>

void Load_FW_Send_Flash_Acknowledgment();
void Load_FW_Receive_Code_Chunks(const char* message, const uint16_t length);
void Load_FW_Flash_Start();

#endif // LOAD_FW_H