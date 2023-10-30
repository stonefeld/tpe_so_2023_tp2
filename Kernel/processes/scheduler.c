#include <interrupts.h>
#include <process.h>
#include <scheduler.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
	int8_t priority;
	ProcessStatus status;
	void* rsp;
} ProcessState;

static void* main_rsp;
static ProcessState processes_states[MAX_PROCESSES];
static int current_running_pid;
static uint8_t quantums;

static int valid_pid(uint64_t pid);
static int valid_priority(int8_t priority);
static int is_active(uint64_t pid);
static int is_ready(uint64_t pid);
static int get_quantums(uint64_t pid);
static uint64_t get_next_ready_pid();
static int try_get_process_state(uint64_t pid, ProcessState** state);

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
	return valid_pid(pid) && processes_states[pid].rsp != NULL;
}

static int
is_ready(uint64_t pid)
{
	return is_active(pid) && processes_states[pid].status == READY;
}

static int
get_quantums(uint64_t pid)
{
	return MIN_PRIORITY - processes_states[pid].priority;
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
try_get_process_state(uint64_t pid, ProcessState** state)
{
	if (!is_active(pid))
		return 0;

	*state = &processes_states[pid];
	return 1;
}

void
sch_init()
{
	current_running_pid = -1;
	quantums = 0;
}

int
sch_on_process_create(int pid,
                      ProcessEntryPoint entry_point,
                      int8_t priority,
                      void* rsp,
                      int argc,
                      const char* const argv[])
{
	if (!valid_priority(priority))
		priority = DEFAULT_PRIORITY;

	processes_states[pid].priority = priority;
	processes_states[pid].status = READY;
	processes_states[pid].rsp = asm_create_process_context(argc, argv, rsp, entry_point);
	return 0;
}

int
sch_block(uint64_t pid)
{
	ProcessState* process_state = NULL;
	if (!try_get_process_state(pid, &process_state))
		return -1;

	process_state->status = BLOCKED;
	if (current_running_pid == pid)
		quantums = 0;
	return 0;
}

int
sch_unblock(uint64_t pid)
{
	ProcessState* process_state = NULL;
	if (!try_get_process_state(pid, &process_state))
		return -1;

	if (process_state->status == READY || process_state->status == RUNNING)
		return 0;

	process_state->status = READY;
	return 0;
}

int
sch_kill(uint64_t pid)
{
	ProcessState* process_state = NULL;
	if (!try_get_process_state(pid, &process_state))
		return -1;

	if (process_state->status == KILLED)
		return 0;

	process_state->status = KILLED;
	process_state->rsp = NULL;

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
	ProcessState* process_state = NULL;
	if (!try_get_process_state(pid, &process_state))
		return -1;

	if (valid_priority(new_priority)) {
		process_state->priority = new_priority;
		return 0;
	}
	return -1;
}

// TODO: revisar
void*
sch_switch(void* current_rsp)
{
	if (current_running_pid >= 0) {
		processes_states[current_running_pid].rsp = current_rsp;
		if (processes_states[current_running_pid].status == RUNNING) {
			processes_states[current_running_pid].status = READY;
		}
	} else if (current_running_pid == -1) {
		main_rsp = current_rsp;
	}

	if (!is_ready(current_running_pid) || quantums == 0) {
		current_running_pid = get_next_ready_pid();

		if (current_running_pid == -1) {
			quantums = 0;
			return main_rsp;
		}

		quantums = get_quantums(current_running_pid);
	} else {
		quantums--;
	}

	processes_states[current_running_pid].status = RUNNING;
	return processes_states[current_running_pid].rsp;
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
sch_get_proc_info(uint64_t pid, Process* info)
{
	ProcessState* process_state = NULL;
	if (!try_get_process_state(pid, &process_state))
		return -1;
	info->status = process_state->status;
	info->priority = process_state->priority;
	info->rsp = process_state->rsp;
	return 0;
}
