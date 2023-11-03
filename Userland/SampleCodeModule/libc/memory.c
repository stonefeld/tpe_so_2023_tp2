#include <memory.h>
#include <syscalls.h>

void*
malloc(size_t size)
{
	return asm_malloc(size);
}

void
free(void* ptr)
{
	asm_free(ptr);
}

void*
realloc(void* ptr, size_t size)
{
	return asm_realloc(ptr, size);
}
