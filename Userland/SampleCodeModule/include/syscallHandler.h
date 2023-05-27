#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#include <stdint.h>

extern uint8_t asm_getchar();
extern void asm_putchar(uint8_t c);
extern void asm_datetime();
extern uint8_t asm_clear();
extern uint8_t asm_setcolor(uint32_t fg, uint32_t bg);
extern void asm_printreg();

#endif
