#include <exceptions.h>
#include <font.h>
#include <keyboard.h>
#include <libasm.h>
#include <memoryManager.h>
#include <pipe.h>
#include <rtc.h>
#include <scheduler.h>
#include <sound.h>
#include <stddef.h>
#include <syscalls.h>
#include <text.h>
#include <time.h>
#include <video.h>

typedef uint64_t (*SyscallHandler)(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static uint64_t exit_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t read_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t write_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t time_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t pipe_create_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t pipe_open_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t pipe_unlink_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t pipe_status_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t malloc_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t free_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t realloc_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

// Las syscalls ahora la idea es que se parezcan a los ids de las de Linux
// Revisar: https://faculty.nps.edu/cseagle/assembly/sys_call.html
// y buscar syscalls equivalentes para poner en dicho id
static SyscallHandler syscalls[] = {
	[1] = exit_handler,         [3] = read_handler,       [4] = write_handler,        [11] = time_handler,
	[42] = pipe_create_handler, [43] = pipe_open_handler, [44] = pipe_unlink_handler, [45] = pipe_status_handler,
	[90] = malloc_handler,      [91] = free_handler,      [92] = realloc_handler,
};

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r9, uint64_t r8)
{
	if (syscalls[rdi] == NULL)
		return -1;
	return syscalls[rdi](rsi, rdx, r10, r9, r8);
}

static uint64_t
exit_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	proc_kill(sch_get_current_pid());
	sch_yield();
	return 0;
}

static uint64_t
read_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	return proc_read(sch_get_current_pid(), rsi, (char*)rdx, rcx);
}

static uint64_t
write_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	return proc_write(sch_get_current_pid(), rsi, (char*)rdx, rcx, r8);
}

static uint64_t
time_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	return 0;
}

static uint64_t
pipe_create_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	int* pipe_fd = (int*)rsi;
	int pid = sch_get_current_pid();
	PipeId pipe_id;
	int fd_rd = -1, fd_wr = -1;

	if ((pipe_id = pipe_create()) < 0 || (fd_rd = pipe_map_fd(pid, -1, pipe_id, 1, 0)) < 0 ||
	    (fd_wr = pipe_map_fd(pid, -1, pipe_id, 0, 1)) < 0) {
		if (fd_rd >= 0)
			proc_unmap_fd(pid, fd_rd);
		if (pipe_id >= 0)
			pipe_free(pipe_id);
		return 1;
	}
	pipe_fd[0] = fd_rd;
	pipe_fd[1] = fd_wr;

	return 0;
}

static uint64_t
pipe_open_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	char* name = (char*)rsi;
	int* pipe_fd = (int*)rdx;
	int pid = sch_get_current_pid();
	PipeId pipe_id;
	int fd_rd = -1, fd_wr = -1;

	if ((pipe_id = pipe_open(name)) < 0 || (fd_rd = pipe_map_fd(pid, -1, pipe_id, 1, 0)) < 0 ||
	    (fd_wr = pipe_map_fd(pid, -1, pipe_id, 0, 1)) < 0) {
		if (fd_rd >= 0)
			proc_unmap_fd(pid, fd_rd);
		if (pipe_id >= 0)
			pipe_free(pipe_id);

		return 1;
	}
	pipe_fd[0] = fd_rd;
	pipe_fd[1] = fd_wr;

	return 0;
}

static uint64_t
pipe_unlink_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	return pipe_unlink((char*)rsi);
}

static uint64_t
pipe_status_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	return pipe_status((PipeStatus*)rsi, rdx);
}

static uint64_t
malloc_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	return (uint64_t)mm_alloc(rsi);
}

static uint64_t
free_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	mm_free((void*)rsi);
	return 0;
}

static uint64_t
realloc_handler(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	return (uint64_t)mm_realloc((void*)rsi, rdx);
}
