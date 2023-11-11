#include <stdint.h>
#include <syscalls.h>
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
		.is_fg = 0,
		.priority = 0,
	};
	return asm_execve(&create_info);
}

int64_t
my_nice(uint64_t pid, uint64_t nice)
{
	return asm_nice(nice, pid);
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

int
my_sem_open(char* name, uint64_t init_value)
{
	return asm_sem_open(name, init_value);
}

int64_t
my_sem_wait(int sem)
{
	return asm_sem_wait(sem);
}

int64_t
my_sem_post(int sem)
{
	return asm_sem_post(sem);
}

int64_t
my_sem_close(int sem)
{
	return asm_sem_close(sem);
}

int64_t
my_yield()
{
	return asm_yield();
}
