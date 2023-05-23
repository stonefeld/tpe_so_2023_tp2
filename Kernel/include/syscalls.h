#ifndef SYSCALLS
#define SYSCALLS

#include <stdint.h>

int sysCallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r9, uint64_t r8);

#endif
