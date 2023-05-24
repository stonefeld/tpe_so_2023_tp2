#ifndef LIB_H
#define LIB_H

#include <stdint.h>

// memory management
void* memset(void* destination, int32_t character, uint64_t length);
void* memcpy(void* destination, const void* source, uint64_t length);

// string manipulation
int strlen(char* str);

// defined in asm/libasm.asm
// everty function starting with `_` is defined in assembly code
extern char* _cpuVendor(char* result);
extern int _rtcGettime(int fmt);
extern char _keyboardActive();
extern int _keyboardGetkey();

#endif
