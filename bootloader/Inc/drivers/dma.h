#ifndef DMA_H
#define DMA_H

#define DMA_BUFFER_SIZE 256
#define DMA_LOG_UART_INSTANCE USART2
#define DMA_LOG_UART_IRQ USART2_IRQn
#define DMA_LOG_UART_TX_STREAM DMA1_Stream6
#define DMA_LOG_UART_TX_IRQ DMA1_Stream6_IRQn
#define DMA_LOG_UART_TX_CHANNEL DMA_SxCR_CHSEL_2 // This selects channel 4 for Stream 6

#define DMA_LOG_UART_RX_STREAM DMA1_Stream5
#define DMA_LOG_UART_RX_CHANNEL DMA_SxCR_CHSEL_2 // This selects channel 4 for Stream 5
#define DMA_LOG_UART_RX_IRQ DMA1_Stream5_IRQn

#include <stdint.h>

void init_log_dma();

uint8_t is_dma_busy();

void DMA1_Stream6_IRQHandler(void);

void initiate_dma_log(const char* message);

uint16_t get_NDTR_RX(void);

char* get_RX_buffer(void);

#endif // DMA_H