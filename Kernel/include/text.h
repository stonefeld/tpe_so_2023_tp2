#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
/*
    Imprime en pantalla un caracter
*/
void tx_put_char(char c, uint32_t color);
/*
    Imprime en pantalla un string
*/
void tx_put_word(char* str, uint32_t color);
/*
    Mueve el cursor ("_") 
*/
void tx_set_cursor(uint32_t x, uint32_t y, uint32_t color);
/*
    Permite activar y desactivar el cursor
*/
void tx_show_cursor(uint8_t show);
/*
    Limpia la pantalla
*/
void tx_clear(uint32_t color);

#endif
