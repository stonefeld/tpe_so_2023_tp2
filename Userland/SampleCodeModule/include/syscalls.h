/*
 * Estas funciones están definidas en libasm/libasm.asm y utilizan
 * syscalls para sus respectivas llamadas.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

typedef int (*EntryPoint)(int argc, char* argv[]);

typedef struct
{
	const char* name;
	EntryPoint entry_point;
	uint8_t is_fg;
	int8_t priority;
	int argc;
	char** argv;
} ProcessCreateInfo;

extern uint32_t asm_exit();
extern uint32_t asm_read(int fd, char* buf, uint32_t size);
extern uint32_t asm_write(int fd, char* buf, uint32_t size, uint32_t color);
extern uint32_t asm_close(int pid, int fd);
extern uint32_t asm_dup(int pid, int fd_from, int fd_to);

extern void asm_time(uint32_t color);
extern void asm_sleep(uint32_t millis);

extern int asm_execve(const ProcessCreateInfo* create_info);
extern int asm_waitpid(int pid);
extern int asm_getpid();
extern int asm_ps(uint32_t color);
extern int asm_nice(int priority, int pid);
extern int asm_kill(int pid);
extern int asm_block(int pid);
extern int asm_yield();

extern uint8_t asm_pipe(int pid, int* pipe_fd);
extern uint8_t asm_pipe_open(int pid, char* name, int* pipe_fd);
extern int asm_pipe_unlink(char* name);
extern int asm_pipe_status(void* status, int size);

extern int asm_sem_open(char* name, uint64_t init_value);
extern int asm_sem_wait(int sem);
extern int asm_sem_post(int sem);
extern int asm_sem_close(int sem);

extern void asm_meminfo(uint32_t color);
extern void* asm_malloc(size_t size);
extern void asm_free(void* ptr);
extern void* asm_realloc(void* ptr, size_t size);

#endif
