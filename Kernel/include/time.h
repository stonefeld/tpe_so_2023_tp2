#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void timer_handler();
uint64_t ticks_elapsed();
uint64_t seconds_elapsed();

#endif
