#include "syscalls.h"

#include <stdint.h>
#include <test_syscalls.h>

int64_t
my_getpid()
{
	return 0;
}

int64_t
my_create_process(char* name, EntryPoint entry_point, uint64_t argc, char* argv[])
{
	ProcessCreateInfo create_info = {
		.argc = argc,
		.argv = argv,
		.name = name,
		.entry_point = entry_point,
		.is_fg = 1,
		.priority = 0,
	};
	return asm_execve(&create_info);
}

int64_t
my_nice(uint64_t pid, uint64_t newPrio)
{
	return 0;
}

int64_t
my_kill(uint64_t pid)
{
	return asm_kill(pid);
}

int64_t
my_block(uint64_t pid)
{
	return asm_block(pid);
}

int64_t
my_unblock(uint64_t pid)
{
	return asm_unblock(pid);
}

int64_t
my_sem_open(char* sem_id, uint64_t initialValue)
{
	return 0;
}

int64_t
my_sem_wait(char* sem_id)
{
	return 0;
}

int64_t
my_sem_post(char* sem_id)
{
	return 0;
}

int64_t
my_sem_close(char* sem_id)
{
	return 0;
}

int64_t
my_yield()
{
	return 0;
}

int64_t
my_wait(int64_t pid)
{
	return 0;
}
