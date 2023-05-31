#include <interrupts.h>
#include <time.h>

static uint64_t ticks = 0, last_ticks = 0;

void
timer_handler()
{
	ticks++;
}

uint64_t
ti_ticks()
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
ti_seconds()
{
	return ticks / 18;
}

void
ti_sleep(uint32_t seconds)
{
	uint32_t last_sec = ti_seconds();
	while (ti_seconds() - last_sec < seconds)
		asm_hlt();
}
