#ifndef LIBASM_H
#define LIBASM_H

// defined in asm/libasm.asm
extern char* _cpuVendor(char* result);
extern int _rtcGettime(int fmt);
extern char _keyboardActive();
extern int _keyboardGetkey();

#endif
