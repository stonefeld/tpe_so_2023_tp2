#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <test_syscalls.h>
#include <test_util.h>
#include <tests.h>

#define SEM_ID               "sem"
#define TOTAL_PAIR_PROCESSES 3

int64_t global;  // shared memory

extern Color color;

void
slow_inc(int64_t* p, int64_t inc)
{
	int aux = *p;
	my_yield();  // This makes the race condition highly probable
	aux += inc;
	*p = aux;
}

int
my_process_inc(int argc, char* argv[])
{
	uint64_t n;
	int sem;
	int8_t inc;
	int8_t use_sem;

	if (argc != 3)
		return -1;

	if ((n = satoi(argv[0])) <= 0)
		return -1;
	if ((inc = satoi(argv[1])) == 0)
		return -1;
	if ((use_sem = satoi(argv[2])) < 0)
		return -1;

	if (use_sem) {
		sem = my_sem_open(SEM_ID, 1);
		if (sem == -1) {
			puts("test_sync: ERROR opening semaphore\n", color.error);
			return -1;
		}
	}

	uint64_t i;
	for (i = 0; i < n; i++) {
		if (use_sem)
			my_sem_wait(sem);
		slow_inc(&global, inc);
		if (use_sem)
			my_sem_post(sem);
	}

	if (use_sem)
		my_sem_close(sem);

	return 0;
}

int
test_sync(int argc, char* argv[])
{
	uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

	if (argc != 2)
		return -1;

	char* argv_dec[] = { argv[0], "-1", argv[1], NULL };
	char* argv_inc[] = { argv[0], "1", argv[1], NULL };

	global = 0;

	uint64_t i;
	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		pids[i] = my_create_process("my_process_inc", my_process_inc, 3, argv_dec);
		pids[i + TOTAL_PAIR_PROCESSES] = my_create_process("my_process_inc", my_process_inc, 3, argv_inc);
	}

	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		asm_waitpid(pids[i]);
		asm_waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
	}

	puts("Final value: ", color.output);
	char buf[256];
	int_to_str(global, buf);
	puts(buf, color.output);
	putchar('\n', color.output);

	return 0;
}
