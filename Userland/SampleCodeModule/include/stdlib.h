#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

uint32_t gets(char* buff, uint32_t size, uint32_t color);
uint8_t getchar(uint8_t* state);
void puts(char* str, uint32_t color);
void putchar(char c, uint32_t color);

uint64_t strlen(char* buff);
uint32_t strtok(char* buff, uint8_t token, char** args, uint32_t size);
uint32_t strcmp(char* s1, char* s2);

uint32_t uint_to_base(uint64_t value, char* buff, uint32_t base);
uint32_t hex_to_uint(char* hexCode);
uint8_t is_hex_color_code(char* code);

#endif
