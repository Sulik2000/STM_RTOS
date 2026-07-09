#ifndef USART2_H
#define USART2_H
#include <stdint.h>

enum USART2_StopBits {
    USART2_STOPBITS_1 = 0,
    USART2_STOPBITS_2 = 1,
    USART2_STOPBITS_0_5 = 2,
};

enum USART2_Parity {
    USART2_PARITY_NONE = 0,
    USART2_PARITY_EVEN = 1,
    USART2_PARITY_ODD = 2,
};

enum USART2_Mode {
    USART2_MODE_TX = 0,
    USART2_MODE_RX = 1,
    USART2_MODE_TX_RX = 2,
};

struct USART2_InitTypeDef {
    uint32_t BaudRate;      // Baud rate
    uint8_t WordLength;    // Word length (8 or 9 bits)
    enum USART2_StopBits StopBits;      // Number of stop bits (1 or 2)
    enum USART2_Parity Parity;        // Parity configuration (none, even, odd)
    uint8_t Mode;          // Mode (TX, RX, or both)
};

void USART2_Init(struct USART2_InitTypeDef *initStruct);
void USART2_Transmit(char *pData, uint16_t Size);
void USART2_Receive(char *pData, uint16_t Size);

#endif // USART2_H