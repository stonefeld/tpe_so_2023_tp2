#ifndef TEST_SYSCALLS_H
#define TEST_SYSCALLS_H

#include <stdint.h>
#include <syscalls.h>

int64_t my_getpid();
int64_t my_create_process(char* name, EntryPoint entry_point, uint64_t argc, char* argv[]);
int64_t my_nice(uint64_t pid, uint64_t nice);
int64_t my_kill(uint64_t pid);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);
int64_t my_yield();

int my_sem_open(char* name, uint64_t init_value);
int64_t my_sem_wait(int sem);
int64_t my_sem_post(int sem);
int64_t my_sem_close(int sem);

#endif
