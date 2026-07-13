#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#define MAX_LOG_MESSAGE_LENGTH 256

/**
 * This function initializes hardware for logging: USART2
 */
void init_log_system();

void log_message(const char* message);

void log_receive_callback(const char* message, const uint16_t length);

#endif // LOG_H