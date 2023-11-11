#ifdef USE_TEST
#include <memoryManager.h>

static void* heap_start;
size_t mem_size;
static void* next_address;

void
mm_init(void* const restrict start_address, size_t size)
{
	heap_start = start_address;
	next_address = heap_start;
	mem_size = (size_t)size;

	return;
}

void*
mm_alloc(const size_t memoryToAllocate)
{
	void* allocation = next_address;
	if (heap_start + mem_size < (next_address + memoryToAllocate)) {
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

#endif
