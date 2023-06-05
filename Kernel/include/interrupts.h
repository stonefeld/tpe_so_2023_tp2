#ifndef INTERRUPS_H
#define INTERRUPS_H

#include <idtLoader.h>

// defined in libasm/interrupts.asm
extern void asm_cli();
extern void asm_sti();
extern void asm_hlt();

extern void asm_pic_master_mask(uint8_t mask);
extern void asm_pic_slave_mask(uint8_t mask);

extern void asm_irq00_handler();
extern void asm_irq01_handler();
extern void asm_irq02_handler();
extern void asm_irq03_handler();
extern void asm_irq04_handler();
extern void asm_irq05_handler();

/*
    Wrappers que llaman a los dispatchers de:
    * syscalls
    * exceptions
*/
extern void asm_syscall_handler();
extern void asm_exception00_handler();
extern void asm_exception06_handler();

#endif
