#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE RED | GREEN | BLUE

uint16_t vd_get_winwidth();
uint16_t vd_get_winheight();
void vd_put_pixel(uint32_t hex_color, uint32_t x, uint32_t y);
void vd_put_pixel_rgb(uint8_t r, uint8_t g, uint32_t b, uint32_t x, uint32_t y);
void vd_put_char(char c, uint32_t x, uint32_t y);
void vd_draw_cursor(uint32_t x, uint32_t y);
void vd_draw_figure(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void vd_scroll_up();
void vd_set_color(uint32_t fg, uint32_t bg);
void vd_clear();
void vd_clear_line(uint32_t y);
void vd_clear_bg(uint32_t bg);
void vd_print_wallpaper(uint32_t size);

#endif
