#include "videoDriver.h"

void
vd_put_pixel(uint32_t hex_color, uint32_t x, uint32_t y)
{
	vd_put_pixel_rgb(hex_color & RED, hex_color & GREEN, hex_color & BLUE, x, y);
}

void
vd_put_pixel_rgb(uint8_t r, uint8_t g, uint32_t b, uint32_t x, uint32_t y)
{
	uint8_t* screen = (uint8_t*)vbe_mode_info->framebuffer;
	int offset = y * vbe_mode_info->pitch + x * vbe_mode_info->bpp / 8;
	screen[offset] = b;
	screen[offset + 1] = g;
	screen[offset + 2] = r;
}
