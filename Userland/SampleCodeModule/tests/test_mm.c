#include "test_syscalls.h"
#include "test_util.h"

#include <stdlib.h>
#include <time.h>
#define MAX_BLOCKS 128

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
	uint64_t max_memory;

	// if (argc != 1)
	// 	return -1;

	// if ((max_memory = satoi(argv[0])) <= 0)
	// 	return -1;
	max_memory = 0x2000000 - 0xF00000 - 1;
	while (1) {
		rq = 0;
		total = 0;

		// Request as many blocks as we can
		while (rq < MAX_BLOCKS && total < max_memory) {
			mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
			mm_rqs[rq].address = malloc(mm_rqs[rq].size);

			if (mm_rqs[rq].address) {
				total += mm_rqs[rq].size;
				rq++;
			}
		}

		char buf[50];
		uint_to_base(total, buf, 10);
		puts("Total: ", 0xffffff);
		puts(buf, 0xffffff);
		uint_to_base(rq, buf, 10);
		puts("\n RQ: ", 0xffffff);
		puts(buf, 0xffffff);
		puts("\n", 0xffffff);

		// Set
		uint32_t i;
		for (i = 0; i < rq; i++)
			if (mm_rqs[i].address)
				memset(mm_rqs[i].address, i, mm_rqs[i].size);

		// Check
		for (i = 0; i < rq; i++)
			if (mm_rqs[i].address)
				if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
					puts("test_mm ERROR\n", 0xffffff);
					return -1;
				}

		// Free
		// for (i = 0; i < rq; i++)
		// 	if (mm_rqs[i].address)
		freeAll(mm_rqs[i].address);

		puts("OK \n", 0xffffff);
		sleep(10);
	}
}