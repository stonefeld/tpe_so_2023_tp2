#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>
#include <test_prio.h>
#include <test_syscalls.h>
#include <test_util.h>

#define MINOR_WAIT 1000000  // TODO: Change this value to prevent a process from flooding the screen
// TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice
#define WAIT 10000000

#define TOTAL_PROCESSES 3
#define LOWEST 19
#define MEDIUM 0
#define HIGHEST -20

int64_t prio[TOTAL_PROCESSES] = { LOWEST, MEDIUM, HIGHEST };

extern Color color;

int
test_prio(int argc, char** argv)
{
	int64_t pids[TOTAL_PROCESSES];
	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++) {
		int pid = my_create_process("endless_loop_print", endless_loop_print, argc, argv);

		if (pid == -1) {
			puts("MURIO\n", 0xffffff);

			for (i = 0; i < TOTAL_PROCESSES; i++)
				my_kill(pids[i]);
			return -1;
		}

		puts("SE CREO UN PROCESO\n", 0xffffff);
		pids[i] = pid;
	}
	asm_ps(color.output);
	bussy_wait(WAIT);

	puts("\nCHANGING PRIORITIES...\n", 0xffffff);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], prio[i]);

	asm_ps(color.output);
	bussy_wait(WAIT);

	puts("\nBLOCKING...\n", 0xffffff);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_block(pids[i]);

	asm_ps(color.output);

	puts("CHANGING PRIORITIES WHILE BLOCKED...\n", 0xffffff);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], MEDIUM);

	asm_ps(color.output);

	puts("UNBLOCKING...\n", 0xffffff);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_unblock(pids[i]);

	asm_ps(color.output);
	bussy_wait(WAIT);

	puts("\nKILLING...\n", 0xffffff);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_kill(pids[i]);

	asm_ps(color.output);
	puts("OK!\n", 0xffffff);
	return 0;
}