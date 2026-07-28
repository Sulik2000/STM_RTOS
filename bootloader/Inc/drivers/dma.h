#ifndef DMA_H
#define DMA_H

#include <stdint.h>

/**
 * Initializes the DMA for logging.
 */
void DMA_Init_log();

/**
 * Checks if the DMA is currently busy.
 */
uint8_t DMA_Is_busy();

/**
 * Sends a log message via DMA.
 */
void DMA_send_log(const char *message, uint16_t length);

/**
 * Gets the current value of the DMA NDTR (Number of Data to Transfer) register.
 */
uint16_t DMA_get_NDTR(void);

/**
 * Gets the current value of the DMA RX buffer.
 */
char* DMA_get_RX_buf(void);

#endif // DMA_H