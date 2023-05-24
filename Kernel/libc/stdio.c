#include <font.h>
#include <libc.h>
#include <stdio.h>
#include <videoDriver.h>

#define WIDTH vd_get_winwidth() / CHAR_WIDTH
#define HEIGHT vd_get_winheight() / CHAR_HEIGHT

static void cursor(uint32_t x, uint32_t y);
static void enter();

static uint32_t curr_x = 0, curr_y = 0;

void
write_buff(char* buff, int size)
{
	while (size-- && *buff != 0) {
		putchar(*buff);
		buff++;
	}
}

void
putchar(char c)
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

		default: {
			vd_put_char(c, curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
			curr_x++;
			if (curr_x >= WIDTH)
				enter();
		} break;
	}
	cursor(curr_x, curr_y);
}

void
puts(char* str)
{
	write_buff(str, strlen(str));
}

static void
cursor(uint32_t x, uint32_t y)
{
	if (x >= WIDTH || x < 0 || y >= HEIGHT || y < 0)
		return;
	vd_draw_cursor(x * CHAR_WIDTH, y * CHAR_HEIGHT);
}

static void
enter()
{
	curr_x = 0;
	curr_y++;
}
