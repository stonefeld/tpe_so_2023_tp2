#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

typedef struct memory_manager_adt* MemoryManager;

void mm_init(void* const restrict start_address, size_t size);

void* mm_alloc(const size_t memoryToAllocate);

void mm_freeAll();
#endif
