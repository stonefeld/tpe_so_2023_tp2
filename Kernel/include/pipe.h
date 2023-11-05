#ifndef PIPE_H
#define PIPE_H
#include <stddef.h>
#define MAX_PID_ARRAY_LENGTH 8
#define MAX_NAME_LEN 16
typedef int PipeId;

typedef struct
{
	int pipe_id;
	size_t bytes_to_read;
	unsigned int readers;
	unsigned int writers;
	int readers_blocked[MAX_PID_ARRAY_LENGTH + 1];
	int writers_blocked[MAX_PID_ARRAY_LENGTH + 1];
	char name[MAX_NAME_LEN + 1];
} PipeStatus;

PipeId create_pipe(void);
PipeId open_pipe(const char* name);
int unlink_pipe(const char* name);
int free_pipe(PipeId id);
size_t write_pipe(PipeId id, void* buf, size_t count);
size_t read_pipe(PipeId id, void* buf, size_t count);
int pipe_map_process(int pid, int fd, PipeId pipe_id, int allow_rd, int allow_wr);
int pipe_status(PipeStatus* status_arr, int max_size);

#endif  // PIPE_H
