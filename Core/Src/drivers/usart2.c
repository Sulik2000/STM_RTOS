#define APB1_CLOCK 16000000.0 // APB1 clock frequency in Hz

#include "drivers/usart2.h"
#include <stm32f411xe.h>

void USART2_Init(struct USART2_InitTypeDef *initStruct) {
    if(initStruct == 0) {
        return;
    }

    // Enable clock for GPIOA
    RCC->AHB1ENR |= (1 << 0);

    // Initiating GPIOA pins for USART2 (PA2 for TX, PA3 for RX)
    GPIOA->MODER &= ~((3 << 4) | (3 << 6));
    GPIOA->MODER |=  ((2 << 4) | (2 << 6)); 

    GPIOA->AFR[0] &= ~((0xF << 8) | (0xF << 12));
    GPIOA->AFR[0] |=  ((7 << 8)   | (7 << 12));

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enable USART2 clock
    USART2->CR1 &= ~(1 << 15); // Select oversampling by 16

    double usartdiv = (APB1_CLOCK) / (16 * initStruct->BaudRate);
    uint16_t mantis = (uint16_t)usartdiv;
    uint16_t fraction = (uint16_t)((usartdiv - mantis) * 16);

    USART2->BRR = (mantis << 4) | (fraction & 0x0F); // Set baud rate
    if(initStruct->WordLength == 9) {
        USART2->CR1 |= (1 << 12); // Set word length to 9 bits
    } else {
        USART2->CR1 &= ~(1 << 12); // Set word length to 8 bits
    }
    
    switch(initStruct->StopBits) {
        case USART2_STOPBITS_1:
            USART2->CR2 &= ~(3 << 12); // 1 stop bit
            break;
        case USART2_STOPBITS_2:
            USART2->CR2 |= (2 << 12); // 2 stop bits
            break;
        case USART2_STOPBITS_0_5:
            USART2->CR2 |= (1 << 12); // 0.5 stop bits
            break;
    }

    switch(initStruct->Parity) {
        case USART2_PARITY_NONE:
            USART2->CR1 &= ~(1 << 10); // No parity
            break;
        case USART2_PARITY_EVEN:
            USART2->CR1 |= (1 << 10); // Enable parity
            USART2->CR1 &= ~(1 << 9); // Even parity
            break;
        case USART2_PARITY_ODD:
            USART2->CR1 |= (1 << 10); // Enable parity
            USART2->CR1 |= (1 << 9); // Odd parity
            break;
    }

    switch(initStruct->Mode) {
        case USART2_MODE_TX:
            USART2->CR1 |= (1 << 3); // Enable transmitter
            break;
        case USART2_MODE_RX:
            USART2->CR1 |= (1 << 2); // Enable receiver
            break;
        case USART2_MODE_TX_RX:
            USART2->CR1 |= (1 << 3) | (1 << 2); // Enable both transmitter and receiver
            break;
    }

    USART2->CR1 |= (1 << 13); // Enable USART2
}

void USART2_Transmit(char *pData, uint16_t Size) {
    for(uint16_t i = 0; i < Size; i++) {
        while(!(USART2->SR & (1 << 7))); // Wait until TXE (Transmit Data Register Empty) is set
        USART2->DR = pData[i]; // Send data
    }
    while(!(USART2->SR & (1 << 6))); // Wait until TC (Transmission Complete) is set
}

void USART2_Receive(char *pData, uint16_t Size) {
    for(uint16_t i = 0; i < Size; i++) {
        while(!(USART2->SR & (1 << 5))); // Wait until RXNE (Read Data Register Not Empty) is set
        pData[i] = USART2->DR; // Read received data
    }
}