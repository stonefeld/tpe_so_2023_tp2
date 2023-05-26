#include <font.h>
#include <libc.h>
#include <text.h>
#include <video.h>

#define WIDTH vd_get_winwidth() / CHAR_WIDTH
#define HEIGHT vd_get_winheight() / CHAR_HEIGHT

static void cursor();
static void enter();

static uint32_t curr_x = 0, curr_y = 0;

void
tx_write_buff(uint8_t* buff, uint64_t size)
{
	while (size-- && *buff != 0) {
		tx_put_char(*buff);
		buff++;
	}
}

void
tx_put_char(uint8_t c)
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
tx_put_word(uint8_t* str)
{
	tx_write_buff(str, strlen(str));
}

void
tx_set_cursor(uint32_t x, uint32_t y)
{
	if (x >= WIDTH || x < 0 || y >= HEIGHT || y < 0)
		return;
	vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
	curr_x = x;
	curr_y = y;
	cursor();
}
void
tx_clear()
{
	vd_clear();
	curr_x=0;
	curr_y=0;
	cursor();	
}

void
tx_put_int(uint64_t x)
{
	char* str = int_to_str(x);
	tx_put_word(str);
}

static void
cursor()
{
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
