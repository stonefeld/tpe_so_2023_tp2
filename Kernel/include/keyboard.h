#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KC_L_SHIFT 42
#define KC_R_SHIFT 54
#define KC_L_SHIFT_RELEASE KC_L_SHIFT + 128
#define KC_R_SHIFT_REREASE KC_R_SHIFT + 128
#define KC_CAPS_LOCK 58
#define KC_L_CTRL 29
#define KC_L_CTRL_RELEASE KC_L_CTRL + 128

#define BUFFER_MAX 10

#define RELEASED 0
#define PRESSED 1

/*
    Maneja la interrupción de hardware del teclado, llena el buffer del teclado  
*/

int keyboard_handler();

/*
*/
char kb_getchar(uint8_t* state);

#endif
