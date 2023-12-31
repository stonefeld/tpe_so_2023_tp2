#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>
#include <test_syscalls.h>
#include <test_util.h>
#include <tests.h>

#define WAIT 1000000000

#define TOTAL_PROCESSES 3
#define LOWEST          19
#define MEDIUM          0
#define HIGHEST         -20

static int64_t prio[TOTAL_PROCESSES] = { LOWEST, MEDIUM, HIGHEST };

extern Color color;

int
test_prio(int argc, char** argv)
{
	int64_t pids[TOTAL_PROCESSES];
	uint64_t i;

	if (argc != 0)
		return -1;

	for (i = 0; i < TOTAL_PROCESSES; i++) {
		int pid = my_create_process("endless_loop_p", endless_loop_print, argc, argv);
		if (pid == -1) {
			for (int j = 0; j < i; j++)
				my_kill(pids[j]);
			return -1;
		}
		pids[i] = pid;
	}
	asm_ps(color.output);
	bussy_wait(WAIT);

	puts("\nCHANGING PRIORITIES...\n", color.output);
	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], prio[i]);

	asm_ps(color.output);
	bussy_wait(WAIT);

	puts("\nBLOCKING...\n", color.output);
	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_block(pids[i]);

	puts("CHANGING PRIORITIES WHILE BLOCKED...\n", color.output);
	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], MEDIUM);

	puts("UNBLOCKING...\n", color.output);
	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_block(pids[i]);

	asm_ps(color.output);
	bussy_wait(WAIT);

	puts("\nKILLING...\n", color.output);
	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_kill(pids[i]);

	puts("OK!\n", color.output);
	return 0;
}
