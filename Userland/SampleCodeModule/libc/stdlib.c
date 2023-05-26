#include <stdint.h>
#include <stdlib.h>
#include <syscallHandler.h>

uint8_t
getchar()
{
	return asm_getchar();
}

void
puts(uint8_t* str)
{
	uint64_t len = strlen(str);
	for (int i = 0; i < len; i++)
		putchar(str[i]);
}

void
putchar(uint8_t c)
{
	asm_putchar(c);
}

uint64_t
strlen(uint8_t* buff)
{
	uint64_t len = 0;
	while (buff[len++] != 0) {}
	return len - 1;
}
