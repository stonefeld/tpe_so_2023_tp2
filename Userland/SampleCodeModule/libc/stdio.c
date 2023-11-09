#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
uint32_t
gets(char* buf, uint32_t size, uint8_t* eof, uint32_t color)
{
	return fgets(STDIN, buf, size, eof, color);
}
uint64_t
printf_color(uint32_t color, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	uint64_t i = 0;
	while (fmt[i]) {
		if (fmt[i] == '%') {
			switch (fmt[++i]) {
				case 'd':;
					char buff[20];
					uint_to_base(va_arg(args, int), buff, 10);
					puts(buff, color);
					break;
				case 's':
					puts(va_arg(args, char*), color);
					break;

				case '%':
					putchar('%', color);
					break;
				default:
					break;
			}
		} else {
			putchar(fmt[i], color);
		}
		i++;
		va_end(args);
	}
	return i;
}
uint64_t
fprintf_color(int fd, uint32_t color, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	uint64_t i = 0;
	while (fmt[i]) {
		if (fmt[i] == '%') {
			switch (fmt[++i]) {
				case 'd':;
					char buff[20];
					uint_to_base(va_arg(args, int), buff, 10);
					puts(buff, color);
					break;
				case 's':
					fputs(fd, va_arg(args, char*), color);
					break;

				case '%':
					fputchar(fd, '%', color);
					break;
				default:
					break;
			}
		} else {
			fputchar(fd, fmt[i], color);
		}
		i++;
		va_end(args);
	}
	return i;
}
uint32_t
fgets(int fd, char* buf, uint32_t size, uint8_t* eof, uint32_t color)
{
	char c;
	uint32_t len = 0;

	while ((c = fgetchar(fd)) >= 0 && c != '\n' && c != '\e') {
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

	*eof = c == '\e';

	if (len != 0)
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
	putchar('\v', color);
}
