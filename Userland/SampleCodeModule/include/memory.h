#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void sleep(int time);

#endif
