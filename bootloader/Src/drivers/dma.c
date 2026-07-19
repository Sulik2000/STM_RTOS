#include "stm32f411xe.h"
#include "drivers/dma.h"
#include "string.h"

char dma_receive_buffer[DMA_BUFFER_SIZE]; // Buffer to store received data

static char dma_transfer_log_buffer[DMA_BUFFER_SIZE];

volatile uint8_t dma_busy_flag = 0;

uint8_t is_dma_busy()
{
    return dma_busy_flag;
}

uint16_t get_NDTR_RX(void){
    return DMA_LOG_UART_RX_STREAM->NDTR;
}

char* get_RX_buffer(void){
    return dma_receive_buffer;
}

static void clear_dma1_stream6_flags(void)
{
    DMA1->HIFCR = DMA_HIFCR_CFEIF6 |
                  DMA_HIFCR_CDMEIF6 |
                  DMA_HIFCR_CTEIF6 |
                  DMA_HIFCR_CHTIF6 |
                  DMA_HIFCR_CTCIF6;
}

void DMA1_Stream6_IRQHandler(void)
{
    // Check if the transfer complete interrupt flag is set
    if (DMA1->HISR & DMA_HISR_TCIF6)
    {
        clear_dma1_stream6_flags();
        dma_busy_flag = 0;
    }
}

void init_log_usart()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN; // Enable DMA1 clock
    DMA_LOG_UART_INSTANCE->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR; // Enable DMA for the log UART RX and TX
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
    while(!(DMA_LOG_UART_TX_STREAM->CR & DMA_SxCR_EN))
    {
        // Wait until the stream is enabled
    }
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
   
    clear_dma1_stream6_flags();
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

    DMA_LOG_UART_TX_STREAM->NDTR = length;
    DMA_LOG_UART_TX_STREAM->M0AR = (uint32_t)dma_transfer_log_buffer;
    DMA_LOG_UART_TX_STREAM->PAR = (uint32_t)&(DMA_LOG_UART_INSTANCE->DR);
    dma_busy_flag = 1;
    enable_dma_log_out_stream();
}

void init_log_dma()
{
    disable_dma_log_out_stream();
    init_log_usart();
    configure_dma_log_stream();
}

void initiate_dma_log(const char *message)
{
    if (is_dma_busy())
    {
        return;
    }

    uint16_t length = str_length(message);
    if (length == 0)
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