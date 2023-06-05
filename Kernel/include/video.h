#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

/*
    Devuelve el ancho de la pantalla
*/
uint16_t vd_get_winwidth();
/*
    Devuelve el largo de la pantalla
*/
uint16_t vd_get_winheight();
void vd_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void vd_put_char(char c, uint32_t x, uint32_t y, uint32_t color);
void vd_draw_cursor(uint32_t x, uint32_t y, uint32_t color);
void vd_draw(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void vd_scroll_up();
void vd_clear(uint32_t bg);
void vd_wallpaper(uint32_t size);

#endif
