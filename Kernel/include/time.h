#ifndef TIME_H
#define TIME_H

#include <stdint.h>

/*
    Maneja la interrupción de hardware del Timer Ticks
*/
void timer_handler();
uint64_t ti_ticks();
uint8_t ti_ticked();
uint64_t ti_seconds();
/*
    Delay 
*/
void ti_sleep(uint32_t millis);

#endif
