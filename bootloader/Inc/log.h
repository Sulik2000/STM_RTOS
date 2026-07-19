#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#define MAX_LOG_MESSAGE_LENGTH 256


/**
 * This function initializes hardware for logging: USART2
 */
void init_log_system();

void log_message(const char* message);

void set_log_receive_callback(void (*callback)(const char* message, const uint16_t length));

uint8_t is_log_system_initialized();

#endif // LOG_H