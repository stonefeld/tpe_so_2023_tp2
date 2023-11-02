
#include <stdlib.h>
#include <test_syscalls.h>
#include <test_util.h>
#include <time.h>

#define MAX_BLOCKS 128
#define MAX_MEMORY (4 * 1024 * 1024)

typedef struct MM_rq
{
	void* address;
	uint32_t size;
} mm_rq;

uint64_t
test_mm()
{
	mm_rq mm_rqs[MAX_BLOCKS];
	uint8_t rq;
	uint32_t total;
	uint64_t max_memory = MAX_MEMORY;

	while (1) {
		rq = 0;
		total = 0;

		// Request as many blocks as we can
		while (rq < MAX_BLOCKS && total < max_memory) {
			mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
			mm_rqs[rq].address = (void*)malloc(mm_rqs[rq].size);
			if (mm_rqs[rq].address) {
				total += mm_rqs[rq].size;
				rq++;
			}
		}

		// Set
		uint32_t i;
		for (i = 0; i < rq; i++)
			if (mm_rqs[i].address)
				setmem(mm_rqs[i].address, i, (size_t)mm_rqs[i].size);

		// Check
		for (i = 0; i < rq; i++)
			if (mm_rqs[i].address)
				if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
					puts(" test_mm ERROR\n", 0xffffff);
					return -1;
				}

		// Free
		for (i = 0; i < rq; i++)
			if (mm_rqs[i].address)
				free(mm_rqs[i].address);

		puts("OK \n", 0xffffff);
		return 0;
	}
}

// b test_mm.c:56
