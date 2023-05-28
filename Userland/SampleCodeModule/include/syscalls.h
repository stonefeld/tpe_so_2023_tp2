#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#define RELEASED 0
#define PRESSED 1

#include <stdint.h>

// defined in libasm/syscalls.asm
// i/o interaciont
extern uint8_t asm_getchar(uint8_t* state);
extern void asm_putchar(uint8_t c);

// drawing
extern void asm_draw(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
extern uint8_t asm_clear();
extern uint8_t asm_setcolor(uint32_t fg, uint32_t bg);
extern void asm_cursor(uint32_t x, uint32_t y);

// properties
extern uint32_t asm_winwidth();
extern uint32_t asm_winheight();
extern uint32_t asm_fontwidth();
extern uint32_t asm_fontheight();

// system
extern uint8_t asm_ticked();
extern void asm_printreg();
extern void asm_datetime();

#endif
