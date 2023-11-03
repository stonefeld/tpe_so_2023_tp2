#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

uint32_t gets(char* buff, uint32_t size, uint32_t color);
uint32_t fgets(int fd, char* buf, uint32_t size, uint32_t color);

char getchar();
char fgetchar(int fd);

uint32_t puts(char* buf, uint32_t color);
uint32_t fputs(int fd, char* buf, uint32_t color);

void putchar(char c, uint32_t color);
void fputchar(int fd, char c, uint32_t color);

void clear(uint32_t color);

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

/*
 * Devuelve si un string es un código hexadecimal válido.
 */
uint8_t is_hex_color_code(char* code);

/*
 * Transforma un código hexadecimal en entero.
 */
uint32_t hex_to_uint(char* code);

void* malloc(size_t memoryToAllocate);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void sleep(int time);

#endif
