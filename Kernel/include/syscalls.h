#ifndef SYSCALLS
#define SYSCALLS

#include <stdint.h>
#define SIZE_REGS 19

/*
    Maneja las syscalls, basicamente una wrapper que llama a los distintos drivers.
*/
uint64_t syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r9, uint64_t r8);

static uint64_t registers [SIZE_REGS]; 
static uint8_t register_flag = 0;
#endif
