#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

void tx_put_char(char c, uint32_t color);
void tx_put_word(char* str, uint32_t color);
void tx_set_cursor(uint32_t x, uint32_t y, uint32_t color);
void tx_show_cursor(uint8_t show);
void tx_clear(uint32_t color);

#endif
