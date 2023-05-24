#include <font.h>
#include <libc.h>
#include <stdio.h>
#include <videoDriver.h>

#define WIDTH vd_get_winwidth() / CHAR_WIDTH
#define HEIGHT vd_get_winheight() / CHAR_HEIGHT

static void backspace();
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
			backspace();
		} break;

		case '\n': {
			enter();
		} break;

		default: {
			vd_put_char(c, curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT);
			curr_x++;
			if (curr_x >= WIDTH)
				enter();
		} break;
	}
}

void
puts(char* str)
{
	write_buff(str, strlen(str));
}

static void
backspace()
{
	if (curr_x == 0 && curr_y == 0)
		return;
	if (curr_x == 0) {
		curr_x = WIDTH;
		curr_y--;
	}
	curr_x--;
	putchar(' ');
	curr_x--;
}

static void
enter()
{
	curr_x = 0;
	curr_y++;
}
