#include <libasm.h>
#include <libc.h>
#include <process.h>
#include <rtc.h>
#include <scheduler.h>
#include <stdint.h>
#include <text.h>

#define ARG_TIMEZONE -3
#define DATE_SIZE    4
#define MONTHS       12

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
static uint8_t monthdays[MONTHS] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static int format(int n);

void
rtc_datetime(uint32_t color)
{
	int32_t day = format(asm_rtc_gettime(DAY));
	int32_t month = format(asm_rtc_gettime(MONTH));
	int32_t year = format(asm_rtc_gettime(YEAR));
	int32_t hours = format(asm_rtc_gettime(HOURS));
	int32_t minutes = format(asm_rtc_gettime(MINUTES));
	int32_t seconds = format(asm_rtc_gettime(SECONDS));
	uint32_t len;

	hours += ARG_TIMEZONE;
	if (hours < 0) {
		hours += 24;
		day--;
		if (day <= 0) {
			month--;
			if (month <= 0) {
				month += 12;
				year--;
			}
			day += monthdays[month - 1];
			if ((year % 400 == 0 || (year % 100 != 0 && year % 4 == 0)) && month == 2)
				day++;
		}
	}

	int pid = sch_get_current_pid();

	len = uint_to_base(year, buff, DEC);
	proc_write(pid, STDOUT, "20", 2, color);
	proc_write(pid, STDOUT, buff, len, color);
	proc_write(pid, STDOUT, "/", 1, color);

	len = uint_to_base(month, buff, DEC);
	if (len == 1)
		proc_write(pid, STDOUT, "0", 1, color);
	proc_write(pid, STDOUT, buff, len, color);
	proc_write(pid, STDOUT, "/", 1, color);

	len = uint_to_base(day, buff, DEC);
	if (len == 1)
		proc_write(pid, STDOUT, "0", 1, color);
	proc_write(pid, STDOUT, buff, len, color);
	proc_write(pid, STDOUT, " ", 1, color);

	len = uint_to_base(hours, buff, DEC);
	if (len == 1)
		proc_write(pid, STDOUT, "0", 1, color);
	proc_write(pid, STDOUT, buff, len, color);
	proc_write(pid, STDOUT, ":", 1, color);

	len = uint_to_base(minutes, buff, DEC);
	if (len == 1)
		proc_write(pid, STDOUT, "0", 1, color);
	proc_write(pid, STDOUT, buff, len, color);
	proc_write(pid, STDOUT, ":", 1, color);

	len = uint_to_base(seconds, buff, DEC);
	if (len == 1)
		proc_write(pid, STDOUT, "0", 1, color);
	proc_write(pid, STDOUT, buff, len, color);
	proc_write(pid, STDOUT, "\n", 1, color);
}

static int
format(int n)
{
	int dec = (n & 240) >> 4;
	int units = n & 15;
	return dec * 10 + units;
}
