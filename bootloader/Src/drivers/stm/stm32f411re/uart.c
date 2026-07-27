#define APB2_CLOCK 16000000.0 // APB2 clock frequency in Hz

#include "drivers/uart.h"
#include <stm32f411xe.h>

uint16_t usart1_receive_index = 0; // Index for the receive buffer
char usart1_receive_buffer[MAX_UART_MESSAGE_LENGTH]; // Buffer to store received data

void (*log_usart_receive_callback)(const char*, uint16_t) = 0;

void UART_Init(struct UART_InitTypeDef *initStruct) {
    if(initStruct == 0) {
        return;
    }

    // Enable clock for GPIOA
    RCC->AHB1ENR |= (1 << 0);

    GPIOA->PUPDR &= ~((3 << 18) | (3 << 20)); // Clear pull-up/pull-down bits for PA9 and PA10
    GPIOA->PUPDR |=  ((1 << 18) | (1 << 20)); // Set PA9 and PA10 to pull-up

    // Initiating GPIOA pins for USART2 (PA9 for TX, PA10 for RX)
    GPIOA->MODER &= ~((3 << 18) | (3 << 20)); // Clear mode bits for PA9 and PA10
    GPIOA->MODER |=  ((2 << 18) | (2 << 20)); // Set PA9 and PA10 to alternate function mode

    GPIOA->AFR[1] &= ~((0xF << 4) | (0xF << 8)); // Clear alternate function bits for PA9 and PA10
    GPIOA->AFR[1] |=  ((0x7 << 4) | (0x7 << 8)); // Set alternate function to AF7 (USART1) for PA9 and PA10

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable USART1 clock
    USART1->CR1 &= ~(1 << 15); // Select oversampling by 16

    double usartdiv = (APB2_CLOCK) / (16 * initStruct->BaudRate);
    uint16_t mantis = (uint16_t)usartdiv;
    uint16_t fraction = (uint16_t)((usartdiv - mantis) * 16);

    USART1->BRR = (mantis << 4) | (fraction & 0x0F); // Set baud rate
    if(initStruct->WordLength == 9) {
        USART1->CR1 |= (1 << 12); // Set word length to 9 bits
    } else {
        USART1->CR1 &= ~(1 << 12); // Set word length to 8 bits
    }
    
    switch(initStruct->StopBits) {
        case UART_STOPBITS_1:
            USART1->CR2 &= ~(3 << 12); // 1 stop bit
            break;
        case UART_STOPBITS_2:
            USART1->CR2 |= (2 << 12); // 2 stop bits
            break;
        case UART_STOPBITS_0_5:
            USART1->CR2 |= (1 << 12); // 0.5 stop bits
            break;
    }

    switch(initStruct->Parity) {
        case UART_PARITY_NONE:
            USART1->CR1 &= ~(1 << 10); // No parity
            break;
        case UART_PARITY_EVEN:
            USART1->CR1 |= (1 << 10); // Enable parity
            USART1->CR1 &= ~(1 << 9); // Even parity
            break;
        case UART_PARITY_ODD:
            USART1->CR1 |= (1 << 10); // Enable parity
            USART1->CR1 |= (1 << 9); // Odd parity
            break;
    }

    switch(initStruct->Mode) {
        case UART_MODE_TX:
            USART1->CR1 |= (1 << 3); // Enable transmitter
            break;
        case UART_MODE_RX:
            USART1->CR1 |= (1 << 2) | USART_CR1_IDLEIE; // Enable receiver
            break;
        case UART_MODE_TX_RX:
            USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_IDLEIE | USART_CR1_RXNEIE; // Activate transmitter, receiver, enable IDLE line detection interrupt, and enable RXNE interrupt
            break;
    }
    USART1->CR1 |= (1 << 13); // Enable USART1
    log_usart_receive_callback = initStruct->ReceiveCallback;
    NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void){
    if(USART1->SR & USART_SR_IDLE) { // Check if IDLE line is detected
        volatile uint32_t tmp; // Temporary variable to clear the IDLE flag
        tmp = USART1->SR; // Read status register
        tmp = USART1->DR; // Read data register to clear the IDLE flag

        if(log_usart_receive_callback) {
            usart1_receive_buffer[usart1_receive_index] = '\0'; // Null-terminate the received string
            log_usart_receive_callback((const char*)usart1_receive_buffer, usart1_receive_index);
            usart1_receive_index = 0; // Reset the index after processing the message
        }
    }

    else if(USART1->SR & USART_SR_RXNE) { // Check if RXNE (Read Data Register Not Empty) is set
        char received_char = USART1->DR;
        if(usart1_receive_index < MAX_UART_MESSAGE_LENGTH - 1) {
            usart1_receive_buffer[usart1_receive_index++] = received_char;
        }
    }
}

void UART_SetReceiveCallback(void (*callback)(const char* message, const uint16_t length)){
    log_usart_receive_callback = callback;
}