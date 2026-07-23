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

uint8_t str_cmp(const char* str1, const char* str2, uint16_t max_length){
    for(uint16_t i = 0; i < max_length; i++){
        if(str1[i] != str2[i]){
            return 0; // Strings are not equal
        }
        if((str1[i] == '\0' && str2[i] != '\0') || (str2[i] == '\0' && str1[i] != '\0')){ // If we reach the end of one string but not the other, strings are not equal
            return 0;
        }
    }
    return 1; // Strings are equal up to max_length
}
