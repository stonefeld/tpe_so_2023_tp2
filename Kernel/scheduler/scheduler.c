#include "scheduler.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
	const char* name;
	void (*entry_point)(int argc, char* argv[]);
	int is_fg;
	int8_t priority;
	int argc;
	const char* const* argv;
} ProcCreateInfo;

typedef struct
{
	int8_t priority;
	Status status;
	void* current_rsp;
} Process;

static void* main_rsp;
static Process processes[MAX_PROCESSES];
static uint64_t current_running_pid;
static uint8_t quantums;

extern void asm_int81();
extern void* asm_create_process_context(int argc,
                                        const char* const argv[],
                                        void* rsp,
                                        void (*entry_point)(int argc, char* argv[]));

static int valid_pid(uint64_t pid);
static int valid_priority(int8_t priority);
static int is_active(uint64_t pid);
static int is_ready(uint64_t pid);
static int get_quantums(uint64_t pid);
static uint64_t get_next_ready_pid();
static int try_get_process(uint64_t pid, Process* outState);

static int
valid_pid(uint64_t pid)
{
	return pid >= 0 && pid < MAX_PROCESSES;
}

static int
valid_priority(int8_t priority)
{
	return priority >= MAX_PRIORITY && priority <= MIN_PRIORITY;
}

static int
is_active(uint64_t pid)
{
	return valid_pid(pid) && processes[pid].current_rsp != NULL;
}

static int
is_ready(uint64_t pid)
{
	return is_active(pid) && processes[pid].status == READY;
}

static int
get_quantums(uint64_t pid)
{
	return MIN_PRIORITY - processes[pid].priority;
}

static uint64_t
get_next_ready_pid()
{
	uint64_t first = current_running_pid < 0 ? 0 : current_running_pid;
	uint64_t next = first;

	do {
		next = (next + 1) % MAX_PROCESSES;
		if (is_ready(next)) {
			return next;
		}
	} while (next != first);

	return -1;
}

static int
try_get_process(uint64_t pid, Process* process)
{
	if (!is_active(pid))
		return 0;

	*process = processes[pid];
	return 1;
}

void
sch_init()
{
	current_running_pid = -1;
	quantums = 0;
}

int
sch_on_process_create(uint64_t pid,
                      void (*entry_point)(int argc, char* argv[]),
                      int8_t priority,
                      void* current_rsp,
                      int argc,
                      const char* const argv[])
{
	if (!valid_priority(priority)) {
		priority = DEFAULT_PRIORITY;
	}

	processes[pid].priority = priority;
	processes[pid].status = READY;
	processes[pid].current_rsp = asm_create_process_context(argc, argv, current_rsp, entry_point);
	return 0;
}

int
sch_block(uint64_t pid)
{
	Process* process = NULL;
	if (!try_get_process(pid, process)) {
		return -1;
	}

	process->status = BLOCKED;
	if (current_running_pid == pid) {
		quantums = 0;
	}
	return 0;
}

int
sch_unblock(uint64_t pid)
{
	Process* process = NULL;
	if (!try_get_process(pid, process))
		return -1;

	if (process->status == READY || process->status == RUNNING)
		return 0;

	process->status = READY;

	return 0;
}

int
sch_kill(uint64_t pid)
{
	Process* process = NULL;
	if (!try_get_process(pid, process))
		return -1;

	if (process->status == KILLED)
		return 0;

	process->status = KILLED;
	process->current_rsp = NULL;

	if (current_running_pid == pid)
		current_running_pid = -1;

	return 0;
}

int
sch_get_current_pid()
{
	return current_running_pid;
}

int
sch_set_priority(int pid, int8_t new_priority)
{
	Process* process = NULL;
	if (!try_get_process(pid, process))
		return -1;

	if (valid_priority(new_priority)) {
		process->priority = new_priority;
		return 0;
	}
	return -1;
}

// Revisar
void*
sch_switch(void* current_rsp)
{
	if (current_running_pid >= 0) {
		processes[current_running_pid].current_rsp = current_rsp;
		if (processes[current_running_pid].status == RUNNING) {
			processes[current_running_pid].status = READY;
		}
	} else if (current_running_pid == -1) {
		main_rsp = current_rsp;
	}

	if (quantums == 0) {
		current_running_pid = get_next_ready_pid();

		if (current_running_pid == -1) {
			quantums = 0;
			return main_rsp;
		}

		quantums = get_quantums(current_running_pid);
	} else {
		quantums--;
	}

	processes[current_running_pid].status = RUNNING;
	return processes[current_running_pid].current_rsp;
}

void
sch_yield()
{
	// Hasta aca llegaste proceso,
	// basta de faltarle el respeto a los otros procesos
	quantums = 0;
	asm_int81();
}

int
sch_get_proc_info(uint64_t pid, ProcInfo* info)
{
	Process* process = NULL;
	if (!try_get_process(pid, process))
		return -1;
	info->status = process->status;
	info->priority = process->priority;
	info->current_rsp = process->current_rsp;
	return 0;
}