#ifndef STRING_H
#define STRING_H
#include <stdint.h>

uint16_t str_length(const char* str);

void str_copy(char* dest, const char* src, uint16_t max_length);

uint8_t str_cmp(const char* str1, const char* str2, uint16_t max_length);

#endif // STRING_H
