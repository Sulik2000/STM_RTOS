#include "log.h"
#include "drivers/dma.h"
#include "drivers/uart.h"

static uint8_t log_system_initialized = 0;
void (*log_receive_callback)(const char* message, const uint16_t length);

void LOG_set_receive_callback(void (*callback)(const char* message, const uint16_t length)){
    log_receive_callback = callback;
    UART_SetReceiveCallback(callback);
}


void LOG_init() {
    struct UART_InitTypeDef uart_init;
    uart_init.BaudRate = 115200;
    uart_init.WordLength = 8;
    uart_init.StopBits = UART_STOPBITS_1;
    uart_init.Parity = UART_PARITY_NONE;
    uart_init.Mode = UART_MODE_TX_RX;
    uart_init.ReceiveCallback = log_receive_callback;

    UART_Init(&uart_init);

    DMA_Init_log();

    log_system_initialized = 1;
}

uint8_t LOG_Is_initialized(){
    return log_system_initialized;
}

void LOG_send(const char* message, uint16_t length) {
    while(DMA_Is_busy()) {
        // Wait for the previous DMA transfer to complete
    }
    DMA_send_log(message, length);
}