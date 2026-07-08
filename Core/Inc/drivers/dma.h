#ifndef DMA_H
#define DMA_H

#define DMA_BUFFER_SIZE 256
#define DMA_LOG_UART_INSTANCE USART2
#define DMA_LOG_UART_IRQ USART2_IRQn
#define DMA_LOG_UART_TX_STREAM DMA1_Stream6

#include <stdint.h>

void init_log_dma();

uint8_t is_dma_busy();

void DMA1_Stream6_IRQHandler(void);

void initiate_dma_log(const char* message);

#endif // DMA_H