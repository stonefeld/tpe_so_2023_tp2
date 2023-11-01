#ifdef USE_LIST

/*
    https://www.geeksforgeeks.org/power-of-two-free-lists-allocators-kernal-memory-allocators/
*/

#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>
#include <stdlib.h>

#define MEMORY_SIZE (9 * 1024 * 1024)  // Memory needed for heap + tree allocation
#define HEAP_SIZE (8 * 1024 * 1024)
#define WORD_ALIGN_DOWN(value) ((value) & (~(size_t)0x07))
#define WORD_ALIGN_UP(value) (WORD_ALIGN_DOWN((size_t)(value) + 7))
#define MIN_BLOCK_SIZE (1 << 8)  // 256 bytes minimum blk
#define USED 1
#define FREE 0

#define BYTE_ALIGNMENT 16

typedef struct Node
{
	size_t size;
	struct Node* next;

} MemoryBlock;

static MemoryBlock blk_list;

static size_t total_mem;
static size_t used_mem;
static unsigned int mem_chuncks;

void
mm_init(void* const restrict start_address, size_t size)
{
	void* true_start = (void*)WORD_ALIGN_UP(start_address);
	size -= (true_start - start_address);

	if (size < MEMORY_SIZE) {
		return;  // fail
	}

	MemoryBlock* first_block = true_start;

	blk_list.next = first_block;

	// initially only one block with full size
	first_block->size = size;
	total_mem = HEAP_SIZE;
}

static void
insert_block(MemoryBlock* block)
{
	MemoryBlock* aux;

	for (aux = &blk_list; aux->next < block && aux->next != NULL; aux = aux->next)
		;

	// encontré donde insertar mi bloque

	if ((aux + aux->size) == block) {  // si son continuos en memoria, los simplifico
		aux->size += block->size;
		block = aux;
	}

	if (aux->next != NULL && block + block->size == aux->next) {  // el siguiente bloque también continuo
		block->size += aux->next->size;
		block->next = aux->next->next;  // could be NULL
		aux->next = block;
	}

	if (block != aux) {
		aux->next = block;
	}
}

static MemoryBlock*
get_block(size_t wanted_size)
{
	MemoryBlock* aux = &blk_list;
	MemoryBlock* block;
	MemoryBlock* prev;

	size_t block_size = wanted_size + sizeof(MemoryBlock);

	for (aux = &blk_list; aux->size < block_size && aux != NULL; aux = aux->next, prev = aux)
		;

	if (aux == NULL) {
		return NULL;  // out of mem
	}

	block = prev->next;
	prev->next = prev->next->next;

	// maybe the block is to large and can be split

	if ((block->size - block_size) > MIN_BLOCK_SIZE) {
		MemoryBlock* free_block = block + block_size;

		free_block->size = block->size - block_size;

		block->size = block_size;

		insert_block(free_block);
	}

	return block;
}

void*
mm_alloc(const size_t memoryToAllocate)
{
	MemoryBlock* block = get_block(memoryToAllocate);

	if (block == NULL) {
		return NULL;  // out of mem
	}

	mem_chuncks += 1;
	used_mem += block->size;

	block->next = &blk_list;

	void* to_return = (void*)(block + sizeof(MemoryBlock));
	return to_return;
}

void
mm_free(void* ptr)
{
	if (ptr == NULL) {
		return;
	}

	MemoryBlock* block = (MemoryBlock*)(ptr - sizeof(MemoryBlock));

	// if it is not allocated

	if (block->next != &blk_list) {
		return;
	}

	insert_block(block);

	mem_chuncks -= 1;
	used_mem -= block->size;

	return;
}

void*
mm_realloc(void* ptr, size_t size)
{
	void* new_ptr = mm_alloc(size);

	if (new_ptr != NULL) {
		memcpy(new_ptr, ptr, size);
		mm_free(ptr);
	}
	return new_ptr;
}

static void
mm_state(MemState* mem_state)
{
	mem_state->total = total_mem;
	mem_state->used = used_mem;
	mem_state->chunks = mem_chuncks;
	mem_state->type = FREE_LIST;
}

#endif
