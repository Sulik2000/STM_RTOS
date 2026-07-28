#ifndef LOAD_FW_H
#define LOAD_FW_H

#include <stdint.h>

#define LOAD_FW_ACKNOWLEDGMENT_MESSAGE ((char[]){0xDD, 0xDD})
#define LOAD_FW_ACKNOWLEDGMENT_MESSAGE_LENGTH 2

#define LOAD_FW_ERROR_MESSAGE ((char[]){0xDD, 0x01})
#define LOAD_FW_ERROR_MESSAGE_LENGTH 2

#define LOAD_FW_UPDATE_COMPLETE_MESSAGE ((char[]){0xFF, 0xFF, 0xDD, 0xDD}) // Value which sent by the host to indicate that the firmware update is complete
#define LOAD_FW_UPDATE_COMPLETE_MESSAGE_LENGTH 4

extern uint32_t _begin_app; // Symbol defined in the linker script, representing the start of the application code

/**
* Sends a message indicating that the code chunk from PC has been received and written to flash memory. After sending the acknowledgment, the bootloader will wait for the next code chunk.
*/
void Load_FW_Send_Flash_Acknowledgment();

/**
 * Receives a code chunk from the PC and writes it to flash memory. If the message indicates that the firmware update is complete, it will finalize the update process.
 */
void Load_FW_Receive_Code_Chunks(const char* message, const uint16_t length);

/**
 * Starts the firmware update process. This function initializes the flash memory and sets up the necessary callbacks to receive code chunks from the PC.
 */
void Load_FW_Flash_Start();

#endif // LOAD_FW_H