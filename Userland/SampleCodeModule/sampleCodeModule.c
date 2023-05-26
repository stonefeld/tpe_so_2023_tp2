#include <stdint.h>
#include <stdlib.h>

int
main(void)
{
	uint8_t c;
	while (1) {
		c = getchar();
		if (c)
			putchar(c);
	}
	return 'c';
}
