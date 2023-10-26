#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#define DEFAULT_PRIORITY 0
#define MAX_PRIORITY -20
#define MIN_PRIORITY 19
#define MAX_NAME 50
#define MAX_PROCESSES 32
// Process with ID=-1 is hlt()

typedef enum
{
	READY = 0,
	RUNNING,
	BLOCKED,
	KILLED
} Status;

typedef struct
{
	uint64_t pid;
	char name[MAX_NAME + 1];
	void* stack_start;
	void* stack_end;
	int is_fg;
	int8_t priority;
	Status status;
	void* current_rsp;
} ProcInfo;

void sch_init();
int sch_on_process_create(uint64_t pid,
                          void (*entry_point)(int argc, char* argv[]),
                          int8_t priority,
                          void* currentRSP,
                          int argc,
                          const char* const argv[]);

int sch_block(uint64_t pid);
int sch_unblock(uint64_t pid);
int sch_kill(uint64_t pid);

int sch_get_current_pid();
int sch_set_priority(int pid, int8_t new_priority);
void* sch_switch(void* current_rsp);
void sch_yield();
int sch_get_proc_info(uint64_t pid, ProcInfo* info);

#endif