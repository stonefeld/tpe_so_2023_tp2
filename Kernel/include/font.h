#ifndef FONT_H
#define FONT_H

#include <stdint.h>

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

uint8_t* fnt_get_char_bitmap(char c);

#endif
