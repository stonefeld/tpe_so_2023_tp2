#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

void tx_write_buff(uint8_t* buff, uint64_t size);
void tx_put_char(uint8_t c);
void tx_put_word(uint8_t* str);
void tx_set_cursor(uint32_t x, uint32_t y);
void tx_put_int(uint64_t x);

#endif
