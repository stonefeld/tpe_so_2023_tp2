#include "memoryManager.h"

struct memory_manager_adt
{
	char* nextAdress;
};

MemoryManager
mm_create(void* const restrict memoryForMemoryManager, void* const restrict managedMemory)
{
	MemoryManager mm = (MemoryManager)memoryForMemoryManager;
	mm->nextAdress = managedMemory;

	return mm;
}

void*
mm_alloc(MemoryManager const restrict self, const size_t memoryToAllocate)
{
	char* allocation = self->nextAdress;
	self->nextAdress += memoryToAllocate;

	return (void*)allocation;
}