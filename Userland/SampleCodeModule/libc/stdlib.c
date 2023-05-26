#include <stdint.h>
#include <stdlib.h>
#include <syscallHandler.h>

uint32_t
gets(uint8_t* buff, uint32_t size)
{
	uint8_t c;
	uint32_t len = 0;

	while ((c = getchar()) != '\n' && len < size - 1) {
		if (c) {
			putchar(c);
			if (c != '\b')
				buff[len++] = c;
			else if (len > 0)
				len--;
		}
	}
	putchar('\n');
	buff[len] = 0;
	return len;
}

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

uint32_t
strtok(uint8_t* buff, uint8_t token, uint8_t** args, uint32_t size)
{
	int args_len = 0;
	if (*buff != token && *buff != 0)
		args[args_len++] = buff;
	while (*buff != 0) {
		if (*buff == token) {
			*buff = 0;
			if (*(buff + 1) != token && *(buff + 1) != 0) {
				if (args_len > size)
					break;
				args[args_len++] = buff + 1;
			}
		}
		buff++;
	}
	return args_len;
}

uint32_t
strcmp(uint8_t* s1, uint8_t* s2)
{
	while (*s1 != 0 && *s2 != 0 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return *s1 == 0 && *s2 == 0;
}
