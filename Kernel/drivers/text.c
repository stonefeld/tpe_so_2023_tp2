#include <font.h>
#include <libc.h>
#include <process.h>
#include <rtc.h>
#include <stddef.h>
#include <stdlib.h>
#include <text.h>
#include <video.h>
#define RED 0xff0000
#define GREEN 0x00ff00
#define BLUE 0x0000ff
#define WHITE RED | GREEN | BLUE

static uint16_t width, height;
static uint32_t curr_x = 0, curr_y = 0;
static uint8_t show_cursor = 1;

static void cursor(uint32_t color);
static void enter();
static int write_callback(int pid, int fd, char* buf, size_t size, uint32_t color);

void
tx_init()
{
	curr_x = curr_y = 0;
	width = vd_get_winwidth() / CHAR_WIDTH;
	height = vd_get_winheight() / CHAR_HEIGHT;
}

void
tx_put_char(char c, uint32_t color)
{
	switch (c) {
		case '\b': {
			if (curr_x == 0 && curr_y == 0)
				return;
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
			if (curr_x == 0) {
				curr_x = width;
				curr_y--;
			}
			curr_x--;
		} break;

		case '\n': {
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
			enter();
		} break;

		case '\t': {
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
			curr_x += 8;
			if (curr_x >= width) {
				uint32_t aux = curr_x - width;
				enter();
				curr_x = aux;
			}
		} break;

		case '\e': {
			tx_clear(color);
		} break;

		default: {
			vd_put_char(c, curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
			curr_x++;
			if (curr_x >= width)
				enter();
		} break;
	}
	cursor(color);
}

void
tx_put_word(char* str, uint32_t color)
{
	while (*str != 0)
		tx_put_char(*str++, color);
}

void
tx_set_cursor(uint32_t x, uint32_t y, uint32_t color)
{
	if (x >= width || x < 0 || y >= height || y < 0)
		return;
	if (show_cursor)
		vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
	curr_x = x;
	curr_y = y;
	cursor(color);
}

void
tx_show_cursor(uint8_t show)
{
	show_cursor = show;
}

void
tx_clear(uint32_t color)
{
	vd_clear(color);
	curr_x = 0;
	curr_y = 0;
	cursor(color);
}

int
tx_map_fd(int pid, int fd)
{
	return proc_map_fd(pid, fd, NULL, write_callback, NULL, NULL);
}

static void
cursor(uint32_t color)
{
	if (show_cursor)
		vd_draw_cursor(curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
}

static void
enter()
{
	curr_x = 0;
	if (curr_y + 1 >= height)
		vd_scroll_up();
	else
		curr_y++;
}

static int
write_callback(int pid, int fd, char* buf, size_t size, uint32_t color)
{
	for (int i = 0; i < size; i++)
		tx_put_char(((char*)buf)[i], color);
	return size;
}
