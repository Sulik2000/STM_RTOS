#ifndef LOG_H
#define LOG_H

#define MAX_LOG_MESSAGE_LENGTH 256

/**
 * This function initializes hardware for logging: USART2
 */
void init_log_system();

void log_message(const char* message);

#endif // LOG_H