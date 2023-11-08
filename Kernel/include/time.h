#ifndef TIME_H
#define TIME_H

#include <stdint.h>

/*
 * Maneja la interrupción de hardware del Timer Ticks
 */
void timer_handler();

uint32_t ti_millis();
void ti_sleep(uint64_t millis);

#endif
