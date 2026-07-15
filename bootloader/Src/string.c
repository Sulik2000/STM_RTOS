#include "string.h"
#include <stdint.h>

uint16_t str_length(const char* str){
    uint16_t length = 0;
    while(str[length] != '\0'){
        length++;
    }
    return length;
}

void str_copy(char* dest, const char* src, uint16_t max_length){
    uint16_t i;
    for(i = 0; i < max_length && src[i] != '\0'; i++){
        dest[i] = src[i];
    }
}