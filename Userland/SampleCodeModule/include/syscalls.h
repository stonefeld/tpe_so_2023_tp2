/*
 * Estas funciones están definidas en libasm/libasm.asm y utilizan
 * syscalls para sus respectivas llamadas.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

typedef void (*ProcessEntryPoint)(int argc, char* argv[]);

typedef struct
{
	const char* name;
	ProcessEntryPoint entry_point;
	uint8_t is_fg;
	int8_t priority;
	int argc;
	char** argv;
} ProcessCreateInfo;

extern uint32_t asm_exit();
extern uint32_t asm_read(int fd, char* buf, uint32_t size);
extern uint32_t asm_write(int fd, char* buf, uint32_t size, uint32_t color);

/*
 * Imprime fecha y hora actual.
 */
extern void asm_time(uint32_t color);

extern int asm_execve(const ProcessCreateInfo* create_info);
extern int asm_getpid();
extern int asm_ps(uint32_t color);
extern int asm_nice(int priority, int pid);
extern int asm_kill(int pid);

extern uint8_t asm_pipe(int* pipe_fd);
extern uint8_t asm_pipe_open(char* name, int* pipe_fd);
extern int asm_pipe_unlink(char* name);
extern int asm_pipe_status(void* status, int size);

extern void* asm_malloc(size_t size);
extern void asm_free(void* ptr);
extern void* asm_realloc(void* ptr, size_t size);

#endif
