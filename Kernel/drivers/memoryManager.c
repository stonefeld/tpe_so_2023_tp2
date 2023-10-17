#include <memoryManager.h>

static void* heap_start;
size_t mem_size;
static void* next_address;

void
mm_init(void* const restrict start_address, size_t size)
{
	heap_start = start_address;
	mem_size = (size_t)size;

	return;
}

void*
mm_alloc(const size_t memoryToAllocate)
{
	void* allocation = next_address;
	if (mem_size < heap_start - (next_address + memoryToAllocate)) {
		return ((void*)0);
	}

	next_address += memoryToAllocate;
	return allocation;
}

void
mm_freeAll()
{
	next_address = heap_start;
	return;
}
// ***********************************************************************************

// struct memory_manager_adt
// {
// 	char* nextAdress;
// };

// static MemoryManager mm;

// mm_init(void* const restrict memoryForMemoryManager, void* const restrict managedMemory)
// {
// 	mm = (MemoryManager)memoryForMemoryManager;
// 	mm->nextAdress = managedMemory;

// 	return mm;
// }

// void*
// mm_alloc(MemoryManager const restrict self, const size_t memoryToAllocate)
// {
// 	char* allocation = self->nextAdress;
// 	self->nextAdress += memoryToAllocate;

// 	return (void*)allocation;
// }