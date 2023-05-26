#include <libasm.h>
#include <rtc.h>
#include <text.h>

#define ARG_TIMEZONE -3

enum time_fmt
{
	SECONDS = 0,
	MINUTES = 2,
	HOURS = 4,
	WEEKDAY = 6,
	DAY,
	MONTH,
	YEAR
};

static int format(int n);

void
rtc_datetime()
{
	tx_put_word((uint8_t*)"Datetime: ");
	tx_put_int(format(asm_rtc_gettime(DAY)));
	tx_put_char('/');
	tx_put_int(format(asm_rtc_gettime(MONTH)));
	tx_put_char('/');
	tx_put_int(format(asm_rtc_gettime(YEAR)));
	tx_put_char(' ');
	tx_put_int(format(asm_rtc_gettime(HOURS)) + ARG_TIMEZONE);
	tx_put_char(':');
	tx_put_int(format(asm_rtc_gettime(MINUTES)));
	tx_put_char(':');
	tx_put_int(format(asm_rtc_gettime(SECONDS)));
	tx_put_char('\n');
}

static int
format(int n)
{
	int dec = (n & 240) >> 4;
	int units = n & 15;
	return dec * 10 + units;
}
