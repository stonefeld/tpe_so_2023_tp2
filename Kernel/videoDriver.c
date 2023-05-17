#include "videoDriver.h"

void
putPixelRGB(uint8_t r, uint8_t g, uint32_t b, uint32_t x, uint32_t y)
{
	uint8_t* screen = vbeModeInfo->framebuffer;
	int offset = y * vbeModeInfo->pitch + x * vbeModeInfo->bpp / 8;
	screen[offset] = b;
	screen[offset + 1] = g;
	screen[offset + 2] = r;
}