#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <process.h>
#include <stdint.h>

#define DEFAULT_PRIORITY 0
#define MAX_PRIORITY -20
#define MIN_PRIORITY 19
#define REALTIME_PRIORITY -5

#define MAX_PROCESSES 32

void sch_init();
int sch_on_process_create(int pid,
                          ProcessEntryPoint entry_point,
                          int8_t priority,
                          void* rsp,
                          int argc,
                          const char* const argv[]);

int sch_block(int pid);
int sch_unblock(int pid);
int sch_on_process_killed(int pid);

int sch_get_current_pid();
int sch_set_priority(int pid, int8_t new_priority);
void* sch_switch(void* current_rsp);
void sch_yield();
int sch_get_proc_info(int pid, Process* info);

#endif
