#include <stdint.h>
#include <time.h>

static uint64_t ticks = 0;

void
timer_handler()
{
	ticks++;
}

int
ticks_elapsed()
{
	return ticks;
}

int
seconds_elapsed()
{
	return ticks / 18;
}
