#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/*
 * Maneja la interrupción de hardware del teclado. Llena el buffer del teclado
 * y guarda los valores en orden de inserción. Realiza el handle para la combinación
 * Ctrl+r que realiza la carga de los registros.
 */
int keyboard_handler();

void kb_init();
int kb_read_chars(char* buf, uint32_t size);

/*
 * Devuelve el siguiente caracter del buffer y el estado del mismo (presionado
 * o soltado).
 */
char kb_getchar(uint8_t* state);

int kb_map_fd(int pid, int fd);

#endif
