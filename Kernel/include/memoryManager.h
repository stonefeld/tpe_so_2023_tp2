#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

typedef struct memory_manager_adt* MemoryManager;

MemoryManager mm_create(void* const restrict memoryForMemoryManager, void* const restrict managedMemory);
void* mm_alloc(MemoryManager const restrict memoryManager, const size_t memoryToAllocate);

#endif
