#ifndef LIBC_H
#define LIBC_H

#include <stdint.h>

// memory management
void* memset(void* destination, int32_t character, uint64_t length);
void* memcpy(void* destination, const void* source, uint64_t length);

// string manipulation
int strlen(uint8_t* str);

// int manipulation
char* int_to_str(uint64_t num);

#endif
