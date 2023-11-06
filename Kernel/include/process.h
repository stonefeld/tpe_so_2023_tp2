#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>

#define MAX_NAME_LEN 256
#define PROCESS_STACK_SIZE 4096
#define MAX_FDS 20

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef void (*ProcessEntryPoint)(int argc, char* argv[]);
typedef int (*ReadCallback)(int pid, int fd, char* buf, uint32_t size);
typedef int (*WriteCallback)(int pid, int fd, char* buf, uint32_t size, uint32_t color);
typedef int (*DupCallback)(int pid_from, int fd_from, int pid_to, int fd_to);
typedef int (*CloseCallback)(int pid, int fd);

typedef enum
{
	READY = 0,
	RUNNING,
	BLOCKED,
	KILLED
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
int proc_kill(int pid);
int proc_list(Process* processes, int max_procs);

int proc_map_fd(int pid,
                int fd,
                ReadCallback read_callback,
                WriteCallback write_callback,
                CloseCallback close_callback,
                DupCallback dup_callback);
int proc_unmap_fd(int pid, int fd);

int proc_read(int pid, int fd, char* buf, uint32_t size);
int proc_write(int pid, int fd, char* buf, uint32_t size, uint32_t color);
int proc_is_fg(int pid);

// asm functions
extern void* asm_create_process_context(int argc, const char* const argv[], void* rsp, ProcessEntryPoint entry_point);

#endif
