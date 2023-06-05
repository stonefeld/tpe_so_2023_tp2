#ifndef LIBASM_H
#define LIBASM_H

#include <stdint.h>

// defined in asm/libasm.asm
extern char* asm_cpu_vendor(char* result);

/*
    Devuelve el ftm (HORA, DIA, etc..) indicado del tiempo medido por RTC.   
*/
extern int asm_rtc_gettime(int fmt);
extern char asm_kbd_active();
extern int asm_kbd_getkey();

/*
    Devuelve el stack pointer actual.
*/
extern uint64_t asm_getsp();
/*
    Guarda en un arreglo el contenido de los registros
*/
extern void asm_setreg();
/*
    Imprime los registros.
*/
extern void asm_printreg(uint32_t color);
extern void asm_sound(uint32_t freq, uint32_t time);
extern void asm_nosound();

#endif
