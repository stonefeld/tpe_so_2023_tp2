#include <interrupts.h>
#include <scheduler.h>
#include <time.h>

static uint64_t ticks = 0;

void
timer_handler()
{
	ticks++;
}

uint32_t
ti_millis()
{
	return ticks * 5000 / 91;
}

void
ti_sleep(uint64_t millis)
{
	uint32_t start = ti_millis();
	while (ti_millis() - start < millis) {
		asm_hlt();
		sch_yield();
	}
}
