#ifndef STRING_H
#define STRING_H
#include <stdint.h>

/**
  * Calculates the length of a null-terminated string. (DON'T USE THIS FOR RAW DATA)
  */
uint16_t str_length(const char* str);

/**
 * Copies a string from the source to the destination buffer, up to a maximum length.
 */
void str_copy(char* dest, const char* src, uint16_t max_length);

/**
 * Compares two strings up to a maximum length. Returns 1 if they are equal, 0 otherwise.
 */
uint8_t str_cmp(const char* str1, const char* str2, uint16_t max_length);

#endif // STRING_H
