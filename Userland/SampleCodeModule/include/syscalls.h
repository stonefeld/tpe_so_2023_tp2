#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#define RELEASED 0
#define PRESSED 1

#include <stdint.h>

// defined in libasm/syscalls.asm
// i/o interaciont
extern uint8_t asm_getchar(uint8_t* state);
extern void asm_putchar(uint8_t c, uint32_t color);

// drawing
extern void asm_draw(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
extern uint8_t asm_clear(uint32_t color);
extern void asm_cursor(uint32_t x, uint32_t y, uint32_t color);
extern void asm_show_cursor(uint8_t show);

// properties
extern void asm_winprops(uint32_t* width, uint32_t* height, uint32_t* font_width, uint32_t* font_height);

// system
extern uint8_t asm_ticked();
extern void asm_sleep(uint32_t ticks);
extern void asm_printreg(uint32_t color);
extern void asm_datetime(uint32_t color);
extern void asm_sound(uint32_t freq, uint32_t duration);

#endif
