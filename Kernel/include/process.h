#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdlib.h>

#define MAX_NAME_LEN 256
#define MAX_PROCESSES 16
#define PROCESS_STACK_SIZE 4096

#define MAX_PRIORITY -20

typedef void (*ProcessEntryPoint)(int argc, char* argv[]);

typedef enum
{
	READY = 0,
	RUNNING,
	BLOCKED
} ProcessStatus;

typedef struct
{
	int pid;
	char name[MAX_NAME_LEN + 1];
	void *stack_end, *stack_start;
	uint8_t is_fg;
	int8_t priority;
	ProcessStatus status;
	void* rsp;
} Process;

typedef struct
{
	const char* name;
	ProcessEntryPoint entry_point;
	uint8_t is_fg;
	int8_t priority;
	int argc;
	char** argv;
} ProcessCreateInfo;

int proc_create(const ProcessCreateInfo* create_info);

#endif
