#include <libasm.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <test_syscalls.h>
#include <test_util.h>

#define BUFSIZE 16

// Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

static char buf[BUFSIZE];

extern Color color;

uint32_t
GetUint()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;
}

uint32_t
GetUniform(uint32_t max)
{
	uint32_t u = GetUint();
	return (u + 1.0) * 2.328306435454494e-10 * max;
}

// Memory
uint8_t
memcheck(void* start, uint8_t value, uint32_t size)
{
	uint8_t* p = (uint8_t*)start;
	uint32_t i;

	for (i = 0; i < size; i++, p++)
		if (*p != value)
			return 0;

	return 1;
}

void*
setmem(void* destiation, int32_t c, size_t length)
{
	uint8_t chr = (uint8_t)c;
	char* dst = (char*)destiation;
	while (length--) {
		dst[length] = chr;
	}
	return destiation;
}

// Parameters
int64_t
satoi(char* str)
{
	uint64_t i = 0;
	int64_t res = 0;
	int8_t sign = 1;

	if (!str)
		return 0;

	if (str[i] == '-') {
		i++;
		sign = -1;
	}

	for (; str[i] != '\0'; ++i) {
		if (str[i] < '0' || str[i] > '9')
			return 0;
		res = res * 10 + str[i] - '0';
	}

	return res * sign;
}

// Dummies
void
bussy_wait(uint64_t n)
{
	uint64_t i;
	for (i = 0; i < n; i++)
		continue;
}

int
endless_loop(int argc, char** argv)
{
	while (1)
		asm_hlt();
}

int
endless_loop_print(int argc, char** argv)
{
	if (argc != 0)
		return -1;

	int pid = asm_getpid();

	while (1) {
		puts("PID: ", color.output);
		uint_to_base(pid, buf, DEC);
		puts(buf, color.output);
		puts("  ", color.output);
		bussy_wait(9000000);
	}
}
