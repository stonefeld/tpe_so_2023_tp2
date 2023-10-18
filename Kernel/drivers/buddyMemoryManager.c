#ifndef USE_TEST

/*
    Siguiendo la explicación de Buddy Memory Allocation: https://en.wikipedia.org/wiki/Buddy_memory_allocation

    Binary Tree Implementation in C : https://www.geeksforgeeks.org/binary-tree-array-implementation/
*/

#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>

#define MIN_LEVEL 0
#define MAX_LEVEL 15 // 16 different orders, so minimum memory block is 256 bytes
#define MIN_BLOCK 256
#define MEMORY_SIZE (9 * 1024 * 1024)  // Memory needed for heap + tree allocation
#define HEAP_SIZE (8 * 1024 * 1024)

#define MAX_NODES 65535

#define WORD_ALIGN_DOWN(value) ((value) & (~(size_t)0x07))
#define WORD_ALIGN_UP(value) (WORD_ALIGN_DOWN((size_t)(value) + 7))

#define POSITIVE_DELTA 1
#define NEGATIVE_DELTA -1
#define USED 1
#define FREE 0
typedef struct
{
	unsigned int used;  // whether this Node is in use or not
	unsigned int used_subnodes;
} Node;

static void* heap_start;

static Node* block_tree;  // binary tree array

static size_t total_mem;
static size_t used_mem;
static unsigned int mem_chuncks;

static inline unsigned int
get_parent(unsigned int index)
{
	return ((index + 1) >> 1) - 1;
}

static inline unsigned int
get_left_child(unsigned int index)
{
	return (index << 1) + 1;
}
static inline unsigned int
get_right_child(unsigned int index)
{
	return (index << 1) + 2;
}
static inline unsigned int
get_first_index(unsigned int level)
{
	return (1 << (MAX_LEVEL - level)) - 1;  // se entienden mucho mejor mirando una imagen del árbol
}

static int
get_first_free_index(unsigned int level)
{
	int index = get_first_index(level);
	int last_index_level = get_first_index(level - 1);
	while (index < last_index_level && (block_tree[index].used || block_tree[index].used_subnodes)) {
		++index;
	}
	if (index == last_index_level)
		return -1;  // no hay un índice libre en ese nivel

	return index;
}

static inline void*
get_ptr(int index, int level)
{
	void * ptr = heap_start + (index - get_first_index(level)) * (1 << level) * MIN_BLOCK;
	return ptr;
}
void
mm_init(void* const restrict start_address, size_t size)
{
	void* true_start = (void*)WORD_ALIGN_UP(start_address);
	size -= (true_start - start_address);

	if (size < MEMORY_SIZE) {
		return;  // fail
	}

	block_tree = true_start;
	size_t tree_size = sizeof(Node) * MAX_NODES;

	true_start += tree_size;
	heap_start = true_start;

	total_mem = HEAP_SIZE;
	used_mem = 0;
	mem_chuncks = 0;  // allocations

	for (int i = 0; i < MAX_NODES; i++) {
		block_tree[i].used = 0;
		block_tree[i].used_subnodes = 0;
	}
}

static unsigned int
get_level(size_t size)
{		
	size--;
	unsigned int level = 0;
	while (size > MIN_BLOCK) {
		size >>= 1;
		level++;
	}
	return level;
}

static void
update_parents(unsigned int index, int delta)
{
	block_tree[index].used_subnodes += delta;  // los subnodes incluyen a sí mismo
	while (index != 0) {
		index = get_parent(index);
		block_tree[index].used_subnodes += delta;
	}
}

static void
set_used(unsigned int index, int state)
{
	// sets refered Node and its descendants as used recursively
	if (index < 0 || index >= MAX_NODES) {
		return;
	}
	block_tree[index].used = state;

	set_used(get_right_child(index), state);
	set_used(get_left_child(index), state);
}
static int
search_node(int index, int* level, void* ptr)
{
	if (*level < MIN_LEVEL)
		return -1;

	if (!block_tree[index].used) {
		(*level)--;
		int right_child = get_right_child(index);
		void* right_ptr = get_ptr(right_child, *level);
		return search_node(ptr < right_ptr ? get_left_child(index) : right_child, level, ptr);
	}
	return ptr == get_ptr(index, *level) ? index : -1;
}

static int
get_initial_index(void* ptr, int max_level)
{
	int offset = ptr - heap_start;
	return get_first_index(max_level) + (offset / ((1 << max_level)*MIN_BLOCK));
}
static int
get_max_level(void* ptr)
{
	int offset = ptr - heap_start;
	int max_level = MAX_LEVEL;

	int max_block_level = (1 << MAX_LEVEL);

	while ( (offset % max_block_level) != 0) {
		max_level--;
		max_block_level = max_block_level >> 1;
	}
	return max_level;
}
void*
mm_alloc(const size_t size)
{
	unsigned int level;
	if (size > HEAP_SIZE || size == 0 || (level = get_level(size)) > MAX_LEVEL ) {
		return NULL;
	}
	
	
	// if (level < MIN_LEVEL)
    //     level = MIN_LEVEL;



	int index = get_first_free_index(level);

	if (index < 0) {
		return NULL;  // out of mem
	}

	update_parents(index, POSITIVE_DELTA);

	set_used(index, USED);

	mem_chuncks++;

	used_mem += (1 << level)*MIN_BLOCK ;

	return get_ptr(index, level);
}
void
mm_free(void* ptr)
{
	if (ptr == NULL || ptr < heap_start || ptr > (heap_start + HEAP_SIZE))
		return;

	// hay que buscar el bloque

	int level = get_max_level(ptr);
	int index = search_node(get_initial_index(ptr, level), &level, ptr);

	if (index < 0) {
		return;  // error
	}

	update_parents(index, NEGATIVE_DELTA);
	set_used(index, FREE);

	mem_chuncks--;
	used_mem -= (1 << level)*MIN_BLOCK ;

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
	mem_state->type = BUDDY;
}
#endif