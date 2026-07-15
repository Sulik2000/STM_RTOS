#define APB1_CLOCK 16000000.0 // APB1 clock frequency in Hz

#include "drivers/usart2.h"
#include <stm32f411xe.h>

uint16_t usart2_receive_index = 0; // Index for the receive buffer
char usart2_receive_buffer[MAX_USART2_MESSAGE_LENGTH]; // Buffer to store received data

void (*log_usart_receive_callback)(const char*, uint16_t) = 0;

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
            USART2->CR1 |= (1 << 2) | USART_CR1_IDLEIE; // Enable receiver
            break;
        case USART2_MODE_TX_RX:
            USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_IDLEIE | USART_CR1_RXNEIE; // Activate transmitter, receiver, enable IDLE line detection interrupt, and enable RXNE interrupt
            break;
    }
    USART2->CR1 |= (1 << 13); // Enable USART2
    log_usart_receive_callback = initStruct->ReceiveCallback;
    NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void){
    if(USART2->SR & USART_SR_RXNE) { // Check if RXNE (Read Data Register Not Empty) is set
        char received_char = USART2->DR;
        if(usart2_receive_index < MAX_USART2_MESSAGE_LENGTH - 1) {
            usart2_receive_buffer[usart2_receive_index++] = received_char;
        }
    }
    if(USART2->SR & USART_SR_IDLE) { // Check if IDLE line is detected
        volatile uint32_t tmp; // Temporary variable to clear the IDLE flag
        tmp = USART2->SR; // Read status register
        tmp = USART2->DR; // Read data register to clear the IDLE flag

        if(log_usart_receive_callback) {
            usart2_receive_buffer[usart2_receive_index] = '\0'; // Null-terminate the received string
            log_usart_receive_callback((const char*)usart2_receive_buffer, usart2_receive_index + 1);
            usart2_receive_index = 0; // Reset the index after processing the message
        }
    }
}

void USART2_SetReceiveCallback(void (*callback)(const char* message, const uint16_t length)){
    log_usart_receive_callback = callback;
}