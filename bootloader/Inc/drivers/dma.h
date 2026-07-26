#ifndef DMA_H
#define DMA_H

#include <stdint.h>

void DMA_Init_log();

uint8_t DMA_Is_busy();

void DMA_send_log(const char* message);

uint16_t DMA_get_NDTR(void);

char* DMA_get_RX_buf(void);

#endif // DMA_H