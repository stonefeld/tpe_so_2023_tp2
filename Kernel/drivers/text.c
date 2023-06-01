#include <font.h>
#include <libc.h>
#include <rtc.h>
#include <text.h>
#include <video.h>

#define WIDTH vd_get_winwidth() / CHAR_WIDTH
#define HEIGHT vd_get_winheight() / CHAR_HEIGHT

static uint32_t curr_x = 0, curr_y = 0;
static uint8_t show_cursor = 1;
static char datetime[DATE_SIZE];

static void cursor();
static void enter();

void
tx_put_char(char c)
{
	switch (c) {
		case '\b': {
			if (curr_x == 0 && curr_y == 0)
				return;
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
			if (curr_x == 0) {
				curr_x = WIDTH;
				curr_y--;
			}
			curr_x--;
		} break;

		case '\n': {
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
			enter();
		} break;

		case '\t': {
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
			curr_x += 8;
			if (curr_x >= WIDTH) {
				uint32_t aux = curr_x - WIDTH;
				enter();
				curr_x = aux;
			}
		} break;

		default: {
			vd_put_char(c, curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
			curr_x++;
			if (curr_x >= WIDTH)
				enter();
		} break;
	}
	cursor();
}

void
tx_put_word(char* str)
{
	while (*str != 0)
		tx_put_char(*str++);
}

void
tx_set_cursor(uint32_t x, uint32_t y)
{
	if (x >= WIDTH || x < 0 || y >= HEIGHT || y < 0)
		return;
	if (show_cursor)
		vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
	curr_x = x;
	curr_y = y;
	cursor();
}

void
tx_show_cursor(uint8_t show)
{
	show_cursor = show;
}

void
tx_clear()
{
	vd_clear();
	curr_x = 0;
	curr_y = 0;
	cursor();
}

void
tx_put_int(uint64_t x)
{
	uint_to_base(x, datetime, DEC);
	tx_put_word(datetime);
}

static void
cursor()
{
	if (show_cursor)
		vd_draw_cursor(curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
}

static void
enter()
{
	curr_x = 0;
	if (curr_y + 1 >= HEIGHT)
		vd_scroll_up();
	else
		curr_y++;
}
