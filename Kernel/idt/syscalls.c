#include "scheduler.h"

#include <exceptions.h>
#include <font.h>
#include <keyboard.h>
#include <libasm.h>
#include <memoryManager.h>
#include <pipe.h>
#include <rtc.h>
#include <sound.h>
#include <stddef.h>
#include <syscalls.h>
#include <text.h>
#include <time.h>
#include <video.h>

#define REGS_SIZE 19

enum syscalls
{
	// i/o interaction
	SYS_READ = 1,
	SYS_WRITE,

	// drawing
	SYS_DRAW,
	SYS_CLEAR,
	SYS_CURSOR,
	SYS_SHOW_CURSOR,

	// properties
	SYS_WINPROPS,

	// system
	SYS_TICKS,
	SYS_SLEEP,
	SYS_REGS,
	SYS_RTC,
	SYS_SOUND,

	// memory
	SYS_MALLOC,
	SYS_FREE,
	SYS_REALLOC,

	// processes
	SYS_EXIT,

	// pipeline
	SYS_PIPE,
	SYS_PIPE_OPEN,
	SYS_PIPE_UNLINK,
	SYS_PIPE_STATUS,
};

static uint8_t regs_flag = 0;
static uint64_t registers[REGS_SIZE];
static int sys_pipe(int pipe_fd[2]);
static int sys_open_pipe(char* name, int pipe_fd[2]);

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {
		case SYS_READ: {
			return proc_read(sch_get_current_pid(), rsi, (char*)rdx, rcx);
		} break;

		case SYS_WRITE: {
			return proc_write(sch_get_current_pid(), rsi, (char*)rdx, rcx, r8);
		} break;

		case SYS_DRAW: {
			vd_draw(rsi, rdx, rcx, r8, r9);
		} break;

		case SYS_CLEAR: {
			tx_clear(rsi);
		} break;

		case SYS_CURSOR: {
			tx_set_cursor(rsi, rdx, rcx);
		} break;

		case SYS_SHOW_CURSOR: {
			tx_show_cursor(rsi);
		} break;

		case SYS_WINPROPS: {
			*((uint32_t*)rsi) = vd_get_winwidth();
			*((uint32_t*)rdx) = vd_get_winheight();
			*((uint32_t*)rcx) = CHAR_WIDTH;
			*((uint32_t*)r8) = CHAR_HEIGHT;
		} break;

		case SYS_TICKS: {
			return ti_ticked();
		} break;

		case SYS_SLEEP: {
			ti_sleep(rsi);
		} break;

		case SYS_REGS: {
			exc_printreg(regs_flag ? registers : NULL, rsi);
		} break;

		case SYS_RTC: {
			rtc_datetime(rsi);
		} break;

		case SYS_SOUND: {
			sd_play(rsi, rdx);
		} break;

		case SYS_MALLOC: {
			return (uint64_t)mm_alloc(rsi);
		} break;

		case SYS_FREE: {
			mm_free((void*)rsi);
		} break;

		case SYS_REALLOC: {
			mm_realloc((void*)rsi, rdx);
		} break;

		case SYS_EXIT: {
			proc_kill(sch_get_current_pid());
			sch_yield();
		} break;
		case SYS_PIPE: {
			return sys_pipe((int*)rsi);
		} break;
		case SYS_PIPE_OPEN: {
			return sys_open_pipe((char*)rsi, (int*)rdx);
		} break;
		case SYS_PIPE_UNLINK: {
			return unlink_pipe((char*)rsi);
		} break;
		case SYS_PIPE_STATUS: {
			return pipe_status((PipeStatus*)rsi, rdx);
		} break;
	}
	return 0;
}

static int
sys_pipe(int pipe_fd[2])
{
	int pid = sch_get_current_pid();

	PipeId pipe_id;

	int fd_rd = -1, fd_wr = -1;
	if ((pipe_id = create_pipe()) < 0 || (fd_rd = pipe_map_process(pid, -1, pipe_id, 1, 0)) < 0 ||
	    (fd_wr = pipe_map_process(pid, -1, pipe_id, 0, 1)) < 0) {
		if (fd_rd >= 0)
			proc_unmap_fd(pid, fd_rd);
		if (pipe_id >= 0)
			free_pipe(pipe_id);

		return 1;
	}
	pipe_fd[0] = fd_rd;
	pipe_fd[1] = fd_wr;

	return 0;
}

static int
sys_open_pipe(char* name, int pipe_fd[2])
{
	int pid = sch_get_current_pid();
	PipeId pipe_id;
	int fd_rd = -1, fd_wr = -1;
	if ((pipe_id = open_pipe(name)) < 0 || (fd_rd = pipe_map_process(pid, -1, pipe_id, 1, 0)) < 0 ||
	    (fd_wr = pipe_map_process(pid, -1, pipe_id, 0, 1)) < 0) {
		if (fd_rd >= 0)
			proc_unmap_fd(pid, fd_rd);
		if (pipe_id >= 0)
			free_pipe(pipe_id);

		return 1;
	}
	pipe_fd[0] = fd_rd;
	pipe_fd[1] = fd_wr;

	return 0;
}
void
save_registers(uint64_t* stack)
{
	regs_flag = 1;
	for (int i = 0; i < REGS_SIZE; i++)
		registers[i] = stack[i];
}
