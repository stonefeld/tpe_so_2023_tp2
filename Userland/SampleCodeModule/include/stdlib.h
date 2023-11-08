#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

#define BIN 2
#define OCT 8
#define DEC 10
#define HEX 16

void* memcpy(void* destination, const void* source, uint64_t length);

/*
 * Devuelve el largo de un string.
 */
uint64_t strlen(char* buff);

/*
 * Parsea un string por token seleccionado.
 */
uint32_t strtok(char* buff, uint8_t token, char** args, uint32_t size);

/*
 * Compara dos strings.
 */
uint32_t strcmp(char* s1, char* s2);

/*
 * Transforma un entero en un string formateado en cierta base.
 */
uint32_t uint_to_base(uint64_t value, char* buff, uint32_t base);
uint32_t int_to_str(int64_t value, char* buff);

/*
 * Devuelve si un string es un código hexadecimal válido.
 */
uint8_t is_hex_color_code(char* code);

/*
 * Transforma un código hexadecimal en entero.
 */
uint32_t hex_to_uint(char* code);

int str_to_int(char* str);

char* str_cat(char* s1, char* s2);

#endif
