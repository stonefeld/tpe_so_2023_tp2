#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>

#define BLACK 0x000000
#define RED   0xff0000
#define GREEN 0x00ff00
#define BLUE  0x0000ff
#define WHITE RED | GREEN | BLUE

#define STDIN  0
#define STDOUT 1
#define STDERR 2

typedef struct
{
	volatile uint32_t fg, bg, output, prompt, error;
} Color;

uint32_t gets(char* buff, uint32_t size, uint8_t* eof, uint32_t color);
uint32_t fgets(int fd, char* buf, uint32_t size, uint8_t* eof, uint32_t color);

char getchar();
char fgetchar(int fd);

uint32_t puts(char* buf, uint32_t color);
uint32_t fputs(int fd, char* buf, uint32_t color);

void putchar(char c, uint32_t color);
void fputchar(int fd, char c, uint32_t color);

void clear(uint32_t color);

#endif
