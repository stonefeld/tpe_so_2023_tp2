#include <time.h>

static uint64_t ticks = 0;
static uint64_t last_ticks = 0;

void
timer_handler()
{
	ticks++;
}

uint64_t
ticks_elapsed()
{
	return ticks;
}

uint8_t
ti_ticked()
{
	if (last_ticks == ticks)
		return 0;
	last_ticks = ticks;
	return 1;
}

uint64_t
seconds_elapsed()
{
	return ticks / 18;
}
