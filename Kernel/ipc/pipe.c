
#include <libc.h>
#include <memoryManager.h>
#include <queue.h>
#include <stddef.h>
#define MAX_PIPES 32
#define BUFFER_MIN_SIZE 512
#define BUFFER_MAX_SIZE (1024 * 4)

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

typedef int PipeId;

static Pipe* pipe_table[MAX_PIPES];

static PipeId
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

// Could return NULL
static Pipe*
get_pipe(PipeId pipe_id)
{
	if (pipe_id >= 0 && pipe_id < MAX_PIPES) {
		return pipe_table[pipe_id];
	}

	return NULL;
}

PipeId
create_pipe()
{
	PipeId id = find_free_pipe();

	if (id == -1)
		return -1;

	Pipe* new_pipe = mm_alloc(sizeof(Pipe));
	Queue rd_q = queue_create();
	Queue wr_q = queue_create();

	if (new_pipe == NULL || rd_q == NULL || wr_q == NULL) {
		mm_free(new_pipe);
		if (rd_q != NULL)
			queue_free(rd_q);
		if (wr_q != NULL)
			queue_free(wr_q);
		return -1;
	}

	memset(new_pipe, 0, sizeof(Pipe));

	new_pipe->rd_q = rd_q;
	new_pipe->wr_q = wr_q;

	pipe_table[id] = new_pipe;

	return id;
}

int
free_pipe(PipeId id)
{
	Pipe* pipe = get_pipe(id);

	if (pipe == NULL) {
		return -1;
	}

	pipe_table[id] = NULL;

	mm_free(pipe->buffer);
	queue_free(pipe->wr_q);
	queue_free(pipe->rd_q);

	return 0;
}
PipeId
open_pipe(const char* name)
{
	return 0;
}

size_t
write_pipe(PipeId id, void* buf, size_t count)
{
	Pipe* pipe = get_pipe(id);
	if (pipe == NULL)
		return -1;
}

size_t
read_pipe(PipeId id, void* buf, size_t count)
{
	Pipe* pipe = get_pipe(id);
	if (pipe == NULL)
		return -1;
}

static size_t
read_pipe_buffer(Pipe pipe, void* buf, size_t count)
{
}

static size_t
write_pipe_buffer(Pipe* pipe, void* buf, size_t count)
{
	size_t required = pipe->stored_bytes + count;

	if (pipe->capacity < required && pipe->capacity < BUFFER_MAX_SIZE) {
		// Amplíamos el tamaño del buffer

		size_t new_capacity = required < BUFFER_MAX_SIZE ? required : BUFFER_MAX_SIZE;

		void* new_buf = mm_alloc(new_capacity);

		if (new_buf != NULL) {
			size_t x = pipe->capacity - pipe->read_idx;
			if (x >= pipe->stored_bytes) {
				memcpy(new_buf, pipe->buffer + pipe->read_idx, pipe->stored_bytes);
			} else {
				memcpy(new_buf, pipe->buffer + pipe->read_idx, x);
				memcpy(new_buf + x, pipe->buffer, pipe->stored_bytes - x);
			}
		}
	}
}