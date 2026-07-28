#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#define MAX_LOG_MESSAGE_LENGTH 256


/**
 * This function initializes hardware for logging, such as UART or USB, and prepares the system to send and receive log messages.
 */
void LOG_init();

/**
 * Sends a log message. The message can be of any length up to MAX_LOG_MESSAGE_LENGTH.
 */
void LOG_send(const char* message, uint16_t length);

/**
 * Sets the callback function that will be called when a log message is received.
 */
void LOG_set_receive_callback(void (*callback)(const char* message, const uint16_t length));

/**
 * Checks if the logging system has been initialized.
 */
uint8_t LOG_Is_initialized();

#endif // LOG_H