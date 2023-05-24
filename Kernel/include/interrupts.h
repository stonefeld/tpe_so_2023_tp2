#ifndef INTERRUPS_H
#define INTERRUPS_H

#include <idtLoader.h>

// defined in libasm/interrupts.asm
void _cli();
void _sti();
void _hlt();

void _picMasterMask(uint8_t mask);
void _picSlaveMask(uint8_t mask);

void _haltcpu();

void _irq00Handler();
void _irq01Handler();
void _irq02Handler();
void _irq03Handler();
void _irq04Handler();
void _irq05Handler();

void _sysCallHandler();
void _exception0Handler();

#endif
