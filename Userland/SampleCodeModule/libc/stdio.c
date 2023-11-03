#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>

uint32_t
gets(char* buf, uint32_t size, uint32_t color)
{
	return fgets(STDIN, buf, size, color);
}

uint32_t
fgets(int fd, char* buf, uint32_t size, uint32_t color)
{
	char c;
	uint32_t len = 0;

	while ((c = fgetchar(fd)) >= 0 && c != '\n') {
		if (c != '\b') {
			if (len < size - 1) {
				putchar(c, color);
				buf[len++] = c;
			}
		} else if (len > 0 && c != '\n') {
			if (buf[len - 1] == '\t')
				for (int i = 0; i < 7; i++)
					putchar(c, color);
			putchar(c, color);
			len--;
		}
	}

	putchar('\n', color);
	buf[len] = 0;
	return len;
}

char
getchar()
{
	return fgetchar(STDIN);
}

char
fgetchar(int fd)
{
	char c;
	if (asm_read(fd, &c, 1) <= 0)
		return -1;
	return c;
}

uint32_t
puts(char* buf, uint32_t color)
{
	return fputs(STDOUT, buf, color);
}

uint32_t
fputs(int fd, char* buf, uint32_t color)
{
	uint64_t len = strlen(buf);
	return asm_write(fd, buf, len, color);
}

void
putchar(char c, uint32_t color)
{
	fputs(STDOUT, &c, color);
}

void
fputchar(int fd, char c, uint32_t color)
{
	fputs(fd, &c, color);
}

void
clear(uint32_t color)
{
	putchar('\e', color);
}
