#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#include <stdint.h>

extern uint8_t asm_getchar();
extern void asm_putchar(uint8_t c);
extern void asm_datetime();
extern uint8_t asm_clear();
extern uint8_t asm_setcolor(uint32_t fg, uint32_t bg);
extern void asm_printreg();
extern void asm_get_ticks();
extern void asm_draw_rectangle(uint8_t fig, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

#endif
