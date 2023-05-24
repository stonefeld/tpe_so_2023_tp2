#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

void tx_write_buff(char* buff, int size);
void tx_put_char(char c);
void tx_put_word(char* str);
void tx_set_cursor(uint32_t x, uint32_t y);

#endif
