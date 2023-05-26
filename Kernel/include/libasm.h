#ifndef LIBASM_H
#define LIBASM_H

// defined in asm/libasm.asm
extern char* asm_cpu_vendor(char* result);
extern int asm_rtc_gettime(int fmt);
extern char asm_kbd_active();
extern int asm_kbd_getkey();

#endif
