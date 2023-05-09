#include <lib.h>
#include <naiveConsole.h>

static char buffer[64] = { '0' };
static uint8_t* const video = (uint8_t*)0xB8000;
static const uint8_t color = 0x07;
static const uint32_t width = 80;
static const uint32_t height = 25;

static uint8_t* currentVideo = video;
static uint8_t currentColor = color;

enum TIME_FMT
{
	SECONDS = 0,
	MINUTES = 2,
	HOURS = 4,
	WEEKDAY = 6,
	DAY = 7,
	MONTH = 8,
	YEAR = 9
};

static uint32_t uintToBase(uint64_t value, char* buffer, uint32_t base);
static int format(int n);

void
ncPrint(const char* string)
{
	int i;
	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void
ncPrintChar(char character)
{
	*currentVideo++ = character;
	*currentVideo++ = currentColor;
}

void
ncNewline()
{
	do {
		ncPrintChar(' ');
	} while ((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void
ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void
ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void
ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void
ncPrintBase(uint64_t value, uint32_t base)
{
	uintToBase(value, buffer, base);
	ncPrint(buffer);
}

void
ncPrintTime()
{
	ncPrint("Datetime: ");
	ncPrintDec(format(getTime(YEAR)));
	ncPrint("/");
	ncPrintDec(format(getTime(MONTH)));
	ncPrint("/");
	ncPrintDec(format(getTime(DAY)));
	ncPrint(" ");
	ncPrintDec(format(getTime(HOURS)));
	ncPrint(":");
	ncPrintDec(format(getTime(MINUTES)));
	ncPrint(":");
	ncPrintDec(format(getTime(SECONDS)));
	ncNewline();
}

void
ncSetColor(unsigned char fg, unsigned char bg)
{
	currentColor = (bg << 4) | (fg & 0x0F);
}

void
ncClear()
{
	int i;
	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
	ncClearColor();
}

void
ncClearColor()
{
	currentColor = color;
}

static uint32_t
uintToBase(uint64_t value, char* buffer, uint32_t base)
{
	char* p = buffer;
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
	p1 = buffer;
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

static int
format(int n)
{
	int dec = (n & 240) >> 4;
	int units = n & 15;
	return dec * 10 + units;
}
