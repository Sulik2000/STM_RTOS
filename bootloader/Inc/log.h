#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#define MAX_LOG_MESSAGE_LENGTH 256


/**
 * This function initializes hardware for logging: USART2
 */
void LOG_init();

void LOG_send(const char* message);

void LOG_set_receive_callback(void (*callback)(const char* message, const uint16_t length));

uint8_t LOG_Is_initialized();

#endif // LOG_H