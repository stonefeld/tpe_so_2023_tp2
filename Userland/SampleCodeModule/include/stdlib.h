#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

uint32_t gets(uint8_t* buff, uint32_t size);
uint8_t getchar();
void puts(uint8_t* str);
void putchar(uint8_t c);

uint64_t strlen(uint8_t* buff);
uint32_t strtok(uint8_t* buff, uint8_t token, uint8_t** args, uint32_t size);
uint32_t strcmp(uint8_t* s1, uint8_t* s2);

#endif
