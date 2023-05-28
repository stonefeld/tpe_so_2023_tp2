#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void timer_handler();
uint64_t ti_ticks();
uint8_t ti_ticked();
uint64_t ti_seconds();

#endif
