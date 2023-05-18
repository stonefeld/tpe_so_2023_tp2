#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void* memset(void* destination, int32_t character, uint64_t length);
void* memcpy(void* destination, const void* source, uint64_t length);

// defined in asm/libasm.asm
extern char* cpu_vendor(char* result);
extern int gettime(int fmt);
extern int getkey();

#endif
