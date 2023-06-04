#include <libasm.h>
#include <libc.h>
#include <rtc.h>
#include <stdint.h>
#include <text.h>

#define ARG_TIMEZONE -3
#define DATE_SIZE 4

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

static char buff[DATE_SIZE];

static int format(int n);

void
rtc_datetime(uint32_t color)
{
	tx_put_word("Datetime: ", color);
	uint32_t value = format(asm_rtc_gettime(DAY));
	uint_to_base(value, buff, DEC);
	tx_put_word(buff, color);
	tx_put_char('/', color);
	value = format(asm_rtc_gettime(MONTH));
	uint_to_base(value, buff, DEC);
	tx_put_word(buff, color);
	tx_put_char('/', color);
	value = format(asm_rtc_gettime(YEAR));
	uint_to_base(value, buff, DEC);
	tx_put_word(buff, color);
	tx_put_char(' ', color);
	value = format(asm_rtc_gettime(HOURS)) + ARG_TIMEZONE;
	uint_to_base(value, buff, DEC);
	tx_put_word(buff, color);
	tx_put_char(':', color);
	value = format(asm_rtc_gettime(MINUTES));
	uint_to_base(value, buff, DEC);
	tx_put_word(buff, color);
	tx_put_char(':', color);
	value = format(asm_rtc_gettime(SECONDS));
	uint_to_base(value, buff, DEC);
	tx_put_word(buff, color);
	tx_put_char('\n', color);
}

static int
format(int n)
{
	int dec = (n & 240) >> 4;
	int units = n & 15;
	return dec * 10 + units;
}
