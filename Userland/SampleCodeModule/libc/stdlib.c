#include <stdint.h>

uint64_t
strlen(char* buff)
{
	uint64_t len = 0;
	while (buff[len++] != 0) {}
	return len - 1;
}

uint32_t
strtok(char* buff, uint8_t token, char** args, uint32_t size)
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
strcmp(char* s1, char* s2)
{
	while (*s1 != 0 && *s2 != 0 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return *s1 == 0 && *s2 == 0;
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

uint8_t
is_hex_color_code(char* code)
{
	uint32_t len = strlen(code);
	if ((len != 7 || code[0] != '#') && (len != 8 || code[0] != '0' || code[1] != 'x'))
		return 0;

	char c;
	for (int i = len == 7 ? 1 : 2; i < len; i++) {
		c = code[i];
		if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')))
			return 0;
	}
	return 1;
}

uint32_t
hex_to_uint(char* code)
{
	uint32_t ret = 0;

	if (code[0] == '0' && code[1] == 'x')
		code += 2;
	else if (code[0] == '#')
		code++;

	for (int i = 0; code[i] != '\0'; i++) {
		char c = code[i];
		uint32_t value = 0;

		if (c >= '0' && c <= '9')
			value = c - '0';
		else if (c >= 'A' && c <= 'F')
			value = 10 + (c - 'A');
		else if (c >= 'a' && c <= 'f')
			value = 10 + (c - 'a');
		else
			return 0;

		ret = (ret << 4) | value;
	}

	return ret;
}
