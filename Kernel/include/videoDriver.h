#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE RED|GREEN|BLUE

void vd_put_pixel(uint32_t hex_color, uint32_t x, uint32_t y);
void vd_put_pixel_rgb(uint8_t r, uint8_t g, uint32_t b, uint32_t x, uint32_t y);

#endif
