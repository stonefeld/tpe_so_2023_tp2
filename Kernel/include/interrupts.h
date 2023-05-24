#ifndef INTERRUPS_H
#define INTERRUPS_H

#include <idtLoader.h>

// defined in libasm/interrupts.asm
extern void _cli();
extern void _sti();
extern void _hlt();

extern void _picMasterMask(uint8_t mask);
extern void _picSlaveMask(uint8_t mask);

extern void _haltcpu();

extern void _irq00Handler();
extern void _irq01Handler();
extern void _irq02Handler();
extern void _irq03Handler();
extern void _irq04Handler();
extern void _irq05Handler();

extern void _sysCallHandler();
extern void _exception0Handler();

#endif
