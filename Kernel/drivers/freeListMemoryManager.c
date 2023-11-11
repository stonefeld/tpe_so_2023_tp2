#ifdef USE_LIST

#include <libc.h>
#include <memoryManager.h>
#include <stdint.h>
#include <stdlib.h>

#define HEAP_MINIMUM_BLOCK_SIZE ((size_t)(struct_size << 1))
#define HEAP_BITS_PER_BYTE ((size_t)8)
#define BYTE_ALIGNMENT 8
#define BYTE_ALIGNMENT_MASK 0x0007

typedef struct Node
{
	struct Node* next;
	size_t block_size;
} MemoryBlock;

static void insert_block(MemoryBlock* block_to_insert);

static const size_t struct_size = sizeof(MemoryBlock);
static MemoryBlock start_block, *end_block = NULL;

static size_t total_mem;
static size_t used_mem;
static unsigned int mem_chunks;

static size_t allocated_bit = 0;

void*
mm_alloc(size_t size)
{
	MemoryBlock* block;
	void* return_ptr = NULL;

	if ((size & allocated_bit) == 0) {
		if ((size > 0) && ((size + struct_size) > size)) {
			size += struct_size;

			if ((size & BYTE_ALIGNMENT_MASK) != 0x00) {
				if ((size + (BYTE_ALIGNMENT - (size & BYTE_ALIGNMENT_MASK))) > size) {
					size += (BYTE_ALIGNMENT - (size & BYTE_ALIGNMENT_MASK));
				} else {
					size = 0;
				}
			}
		} else {
			size = 0;
		}

		if ((size > 0) && (size <= (total_mem - used_mem))) {
			MemoryBlock* previous_block = &start_block;
			block = start_block.next;

			while ((block->block_size < size) && (block->next != NULL)) {
				previous_block = block;
				block = block->next;
			}

			if (block != end_block) {
				return_ptr = (void*)(((uint8_t*)previous_block->next) + struct_size);
				previous_block->next = block->next;

				if ((block->block_size - size) > HEAP_MINIMUM_BLOCK_SIZE) {
					MemoryBlock* new_block_link = (void*)(((uint8_t*)block) + size);
					new_block_link->block_size = block->block_size - size;
					block->block_size = size;
					insert_block(new_block_link);
				}

				used_mem += block->block_size;
				block->block_size |= allocated_bit;
				block->next = NULL;
				mem_chunks++;
			}
		}
	}
	return return_ptr;
}

void
mm_free(void* address)
{
	uint8_t* puc = (uint8_t*)address;
	MemoryBlock* link;

	if (address != NULL) {
		puc -= struct_size;
		link = (void*)puc;

		if ((link->block_size & allocated_bit) != 0) {
			if (link->next == NULL) {
				link->block_size &= ~allocated_bit;
				used_mem -= link->block_size;
				insert_block(((MemoryBlock*)link));
				mem_chunks--;
			}
		}
	}
}

void
mm_init(void* const restrict start_address, size_t size)
{
	MemoryBlock* block;
	void* true_start_address;
	size_t true_start;
	size_t total_heap_size = size;

	true_start = (size_t)start_address;
	if ((true_start & BYTE_ALIGNMENT_MASK) != 0) {
		true_start += (BYTE_ALIGNMENT - 1);
		true_start &= ~((size_t)BYTE_ALIGNMENT_MASK);
		total_heap_size -= true_start - (size_t)start_address;
	}

	true_start_address = (uint8_t*)true_start;
	start_block.next = (void*)true_start_address;
	start_block.block_size = (size_t)0;

	true_start = ((size_t)true_start_address) + total_heap_size;
	true_start -= struct_size;
	true_start &= ~((size_t)BYTE_ALIGNMENT_MASK);
	end_block = (void*)true_start;
	end_block->block_size = 0;
	end_block->next = NULL;

	block = (void*)true_start_address;
	block->block_size = true_start - (size_t)block;
	block->next = end_block;

	total_mem = block->block_size;
	allocated_bit = ((size_t)1) << ((sizeof(size_t) * HEAP_BITS_PER_BYTE) - 1);
}

static void
insert_block(MemoryBlock* block_to_insert)
{
	MemoryBlock* iterator;
	uint8_t* puc;

	for (iterator = &start_block; iterator->next < block_to_insert; iterator = iterator->next) {}

	puc = (uint8_t*)iterator;
	if ((puc + iterator->block_size) == (uint8_t*)block_to_insert) {
		iterator->block_size += block_to_insert->block_size;
		block_to_insert = iterator;
	}

	puc = (uint8_t*)block_to_insert;
	if ((puc + block_to_insert->block_size) == (uint8_t*)iterator->next) {
		if (iterator->next != end_block) {
			block_to_insert->block_size += iterator->next->block_size;
			block_to_insert->next = iterator->next->next;
		} else {
			block_to_insert->next = end_block;
		}
	} else {
		block_to_insert->next = iterator->next;
	}

	if (iterator != block_to_insert) {
		iterator->next = block_to_insert;
	}
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

void
mm_state(MemState* mem_state)
{
	mem_state->total = total_mem;
	mem_state->used = used_mem;
	mem_state->chunks = mem_chunks;
	mem_state->type = FREE_LIST;
}

#endif
