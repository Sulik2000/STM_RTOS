#include "stm32f411xe.h"
#include "drivers/dma.h"
#include "string.h"

#define DMA_BUFFER_SIZE 256
#define DMA_LOG_UART_INSTANCE USART1
#define DMA_LOG_UART_IRQ USART1_IRQn
#define DMA_LOG_UART_TX_STREAM DMA2_Stream7
#define DMA_LOG_UART_TX_IRQ DMA2_Stream7_IRQn
#define DMA_LOG_UART_TX_CHANNEL (0b100 << 25) // This selects channel 4

#define DMA_LOG_UART_RX_STREAM DMA2_Stream5
#define DMA_LOG_UART_RX_CHANNEL (0b100 << 25) // This selects channel 4
#define DMA_LOG_UART_RX_IRQ DMA2_Stream5_IRQn

char dma_receive_buffer[DMA_BUFFER_SIZE]; // Buffer to store received data

static char dma_transfer_log_buffer[DMA_BUFFER_SIZE];

volatile uint8_t dma_busy_flag = 0;

uint8_t DMA_Is_busy()
{
    return dma_busy_flag;
}

uint16_t DMA_get_NDTR(void){
    return DMA_LOG_UART_RX_STREAM->NDTR;
}

char* DMA_get_RX_buf(void){
    return dma_receive_buffer;
}

static void clear_dma2_stream7_flags(void)
{
    DMA2->HIFCR = DMA_HIFCR_CFEIF7 |
                  DMA_HIFCR_CDMEIF7 |
                  DMA_HIFCR_CTEIF7 |
                  DMA_HIFCR_CHTIF7 |
                  DMA_HIFCR_CTCIF7;
}

void DMA2_Stream7_IRQHandler(void)
{
    // Check if the transfer complete interrupt flag is set
    if (DMA2->HISR & DMA_HISR_TCIF7)
    {
        clear_dma2_stream7_flags();
        dma_busy_flag = 0;
    }
}

void DMA_Init_registers()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN; // Enable DMA2 clock
    DMA_LOG_UART_INSTANCE->CR3 |= USART_CR3_DMAT; // Enable DMA for the log UART RX and TX
}

void disable_dma_log_out_stream()
{
    DMA_LOG_UART_TX_STREAM->CR &= ~DMA_SxCR_EN; // Disable the DMA stream
    while(DMA_LOG_UART_TX_STREAM->CR & DMA_SxCR_EN)
    {
        // Wait until the stream is disabled
    }
}

void enable_dma_log_out_stream()
{
    DMA_LOG_UART_TX_STREAM->CR |= DMA_SxCR_EN; // Enable the DMA stream
}

void disable_dma_log_in_stream()
{
    DMA_LOG_UART_RX_STREAM->CR &= ~DMA_SxCR_EN; // Disable the DMA stream
    while(DMA_LOG_UART_RX_STREAM->CR & DMA_SxCR_EN)
    {
        // Wait until the stream is disabled
    }
}

void enable_dma_log_in_stream()
{
    DMA_LOG_UART_RX_STREAM->CR |= DMA_SxCR_EN; // Enable the DMA stream
    while(!(DMA_LOG_UART_RX_STREAM->CR & DMA_SxCR_EN))
    {
        // Wait until the stream is enabled
    }
}

static void configure_dma_log_stream(void)
{
    DMA_LOG_UART_TX_STREAM->CR &= ~DMA_SxCR_EN;
    while (DMA_LOG_UART_TX_STREAM->CR & DMA_SxCR_EN)
    {
    }

    DMA_LOG_UART_TX_STREAM->CR = 0;
    DMA_LOG_UART_TX_STREAM->FCR = 0;

    DMA_LOG_UART_TX_STREAM->CR = DMA_LOG_UART_TX_CHANNEL |
                                 DMA_SxCR_MINC | // Memory increment mode
                                 DMA_SxCR_DIR_0 | // Memory-to-peripheral direction
                                 DMA_SxCR_TCIE | // Transfer complete interrupt enable
                                 DMA_SxCR_PL_1; // High priority
   
    clear_dma2_stream7_flags();
    NVIC_ClearPendingIRQ(DMA_LOG_UART_TX_IRQ);
    NVIC_EnableIRQ(DMA_LOG_UART_TX_IRQ);

    NVIC_ClearPendingIRQ(DMA_LOG_UART_RX_IRQ);
    NVIC_EnableIRQ(DMA_LOG_UART_RX_IRQ);
}

void start_dma_log_transfer(uint16_t length)
{
    if (length == 0 || length > DMA_BUFFER_SIZE || dma_busy_flag)
    {
        return;
    }

    disable_dma_log_out_stream();

    clear_dma2_stream7_flags();
    DMA_LOG_UART_INSTANCE->SR &= ~USART_SR_TC;

    DMA_LOG_UART_TX_STREAM->NDTR = length;
    DMA_LOG_UART_TX_STREAM->M0AR = (uint32_t)dma_transfer_log_buffer;
    DMA_LOG_UART_TX_STREAM->PAR = (uint32_t)&(DMA_LOG_UART_INSTANCE->DR);
    dma_busy_flag = 1;
    enable_dma_log_out_stream();
}

void DMA_Init_log()
{
    DMA_Init_registers();
    disable_dma_log_out_stream();
    configure_dma_log_stream();
}

void DMA_send_log(const char *message, uint16_t length)
{
    if (DMA_Is_busy() || length == 0)
    {
        return;
    }

    if (length > DMA_BUFFER_SIZE)
    {
        length = DMA_BUFFER_SIZE - 1;
    }

    str_copy(dma_transfer_log_buffer, message, length);

    start_dma_log_transfer(length);
}