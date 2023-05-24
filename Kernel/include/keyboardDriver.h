#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KC_ESC 27
#define KC_L_SHIFT 42
#define KC_R_SHIFT 54
#define KC_L_SHIFT_RELEASE KC_L_SHIFT + 128
#define KC_R_SHIFT_REREASE KC_R_SHIFT + 128
#define KC_CAPS_LOCK 58
#define BUFFER_MAX 10

uint8_t kb_getkey();
int keyboard_handler();

#endif
