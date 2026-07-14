#include "log.h"
#include "drivers/dma.h"
#include "drivers/usart2.h"

void init_log_system() {
    struct USART2_InitTypeDef usart2_init;
    usart2_init.BaudRate = 115200;
    usart2_init.WordLength = 8;
    usart2_init.StopBits = USART2_STOPBITS_1;
    usart2_init.Parity = USART2_PARITY_NONE;
    usart2_init.Mode = USART2_MODE_TX_RX;
    usart2_init.ReceiveCallback = log_receive_callback;

    USART2_Init(&usart2_init);

    init_log_dma();
}

void log_message(const char* message) {
    while(is_dma_busy()) {
        // Wait for the previous DMA transfer to complete
    }
    initiate_dma_log(message);
}

void log_receive_callback(const char* message, const uint16_t length){
    initiate_dma_log(message); // For tests
}