#include <keyboardDriver.h>
#include <stdint.h>
#include <time.h>

void
irq_dispatcher(uint64_t irq)
{
	switch (irq) {
		case 0: {
			timer_handler();
		} break;

		case 1: {
			keyboard_handler();
		} break;
	}
	return;
}
