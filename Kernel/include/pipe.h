#ifndef PIPE_H
#define PIPE_H

#include <stddef.h>

#define MAX_PID_ARRAY_LENGTH 8
#define MAX_NAME_LENGHT 16

typedef int PipeId;

typedef struct
{
	int pipe_id;
	size_t bytes_to_read;
	unsigned int readers;
	unsigned int writers;
	int readers_blocked[MAX_PID_ARRAY_LENGTH + 1];
	int writers_blocked[MAX_PID_ARRAY_LENGTH + 1];
	char name[MAX_NAME_LENGHT + 1];
} PipeStatus;

PipeId pipe_create();
PipeId pipe_open(char* name);
int pipe_unlink(char* name);
int pipe_free(PipeId id);
size_t pipe_write(PipeId id, char* buf, size_t count);
size_t pipe_read(PipeId id, char* buf, size_t count);
int pipe_map_fd(int pid, int fd, PipeId pipe_id, int allow_rd, int allow_wr);
int pipe_status(PipeStatus* status_arr, int max_size);

#endif
