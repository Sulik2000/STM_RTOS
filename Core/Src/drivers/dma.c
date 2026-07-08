#include "stm32f411xe.h"
#include "drivers/dma.h"
#include "string.h"

static char dma_log_buffer[DMA_BUFFER_SIZE];

volatile uint8_t dma_busy_flag = 0;

uint8_t is_dma_busy()
{
    return dma_busy_flag;
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
    DMA_LOG_UART_INSTANCE->CR3 |= USART_CR3_DMAT; // Enable DMA for the log UART
}

void disable_dma_log_stream()
{
    DMA_LOG_UART_TX_STREAM->CR &= ~DMA_SxCR_EN; // Disable the DMA stream
}

void enable_dma_log_stream()
{
    DMA_LOG_UART_TX_STREAM->CR |= DMA_SxCR_EN; // Enable the DMA stream
}

static void configure_dma_log_stream(void)
{
    DMA_LOG_UART_TX_STREAM->CR &= ~DMA_SxCR_EN;
    while (DMA_LOG_UART_TX_STREAM->CR & DMA_SxCR_EN)
    {
    }

    DMA_LOG_UART_TX_STREAM->CR = 0;
    DMA_LOG_UART_TX_STREAM->FCR = 0;
    DMA_LOG_UART_TX_STREAM->CR = DMA_SxCR_CHSEL_2 |
                                 DMA_SxCR_MINC |
                                 DMA_SxCR_DIR_0 |
                                 DMA_SxCR_TCIE |
                                 DMA_SxCR_PL_1;

    clear_dma1_stream6_flags();
    NVIC_ClearPendingIRQ(DMA1_Stream6_IRQn);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

void start_dma_log_transfer(uint16_t length)
{
    if (length == 0 || length > DMA_BUFFER_SIZE || dma_busy_flag)
    {
        return;
    }

    DMA_LOG_UART_TX_STREAM->NDTR = length;
    DMA_LOG_UART_TX_STREAM->M0AR = (uint32_t)dma_log_buffer;
    DMA_LOG_UART_TX_STREAM->PAR = (uint32_t)&(DMA_LOG_UART_INSTANCE->DR);
    dma_busy_flag = 1;
    enable_dma_log_stream();
}

void init_log_dma()
{
    disable_dma_log_stream();
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

    str_copy(dma_log_buffer, message, length + 1);

    start_dma_log_transfer(length);
}