#ifndef UART_H
#define UART_H
#include <stdint.h>

#define MAX_UART_MESSAGE_LENGTH 256

enum UART_StopBits {
    UART_STOPBITS_1 = 0,
    UART_STOPBITS_2 = 1,
    UART_STOPBITS_0_5 = 2,
};

enum UART_Parity {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 1,
    UART_PARITY_ODD = 2,
};

enum UART_Mode {
    UART_MODE_TX = 0,
    UART_MODE_RX = 1,
    UART_MODE_TX_RX = 2,
};

struct UART_InitTypeDef {
    uint32_t BaudRate;      // Baud rate
    uint8_t WordLength;    // Word length (8 or 9 bits)
    enum UART_StopBits StopBits;      // Number of stop bits (1 or 2)
    enum UART_Parity Parity;        // Parity configuration (none, even, odd)
    enum UART_Mode Mode;          // Mode (TX, RX, or both)
    void (*ReceiveCallback)(const char* message, const uint16_t length); // Callback function for received data
};

void UART_Init(struct UART_InitTypeDef *initStruct);
void UART_Transmit(char *pData, uint16_t Size);
void UART_Receive(char *pData, uint16_t Size);
void UART_SetReceiveCallback(void (*callback)(const char* message, const uint16_t length));

#endif // UART_H