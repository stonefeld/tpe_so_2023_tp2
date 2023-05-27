#include "libc.h"

#include <stdint.h>

void*
memset(void* destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char* dst = (char*)destination;

	while (length--)
		dst[length] = chr;

	return destination;
}

void*
memcpy(void* destination, const void* source, uint64_t length)
{
	/*
	 * memcpy does not support overlapping buffers, so always do it
	 * forwards. (Don't change this without adjusting memmove.)
	 *
	 * For speedy copying, optimize the common case where both pointers
	 * and the length are word-aligned, and copy word-at-a-time instead
	 * of byte-at-a-time. Otherwise, copy by bytes.
	 *
	 * The alignment logic below should be portable. We rely on
	 * the compiler to be reasonably intelligent about optimizing
	 * the divides and modulos out. Fortunately, it is.
	 */
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 && (uint64_t)source % sizeof(uint32_t) == 0 &&
	    length % sizeof(uint32_t) == 0) {
		uint32_t* d = (uint32_t*)destination;
		const uint32_t* s = (const uint32_t*)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	} else {
		uint8_t* d = (uint8_t*)destination;
		const uint8_t* s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

uint32_t
strlen(char* str)
{
	int len = 0;
	while (str[len++] != 0) {}
	return len - 1;
}

uint32_t
uint_to_base(uint64_t value, char* buff, uint32_t base)
{
	char* p = buff;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buff;
	p2 = p - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void
int_to_str(uint64_t num, char* buff, uint32_t size)
{
	int i, resto, len = 0, n;

	n = num;
	while (n != 0) {
		len++;
		n /= 10;
	}

	for (i = 0; i < len; i++) {
		resto = num % 10;
		num = num / 10;
		buff[len - (i + 1)] = resto + '0';
	}
	buff[len] = '\0';
}
