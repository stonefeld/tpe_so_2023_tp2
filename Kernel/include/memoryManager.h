#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

typedef enum
{
	FREE_LIST,
	BUDDY
} MemType;

typedef struct
{
	MemType type;
	size_t total;
	size_t used;
	unsigned int chunks;
} MemState;

void mm_init(void* const restrict start_address, size_t size);
void* mm_alloc(const size_t memoryToAllocate);
void mm_free(void* ptr);
void* mm_realloc(void* ptr, size_t size);

#endif
