
#include <queue.h>
#include <stddef.h>
#define MAX_PIPES 32

typedef struct
{
	void* buffer;

	size_t capacity;

	size_t read_idx;

	size_t stored_bytes;

	unsigned int readers, writers;

	Queue rd_q, wr_q;

	const char* name;

} Pipe;

typedef struct
{
	int pipe_id;
	int allow_rd, allow_wr;

} PipeFd;

static Pipe* pipe_table[MAX_PIPES];

static int
find_free_pipe(void)
{
	int i;
	for (i = 0; i < MAX_PIPES; i++) {
		if (pipe_table[i] == NULL) {
			return i;
		}
	}
	return -1;
}