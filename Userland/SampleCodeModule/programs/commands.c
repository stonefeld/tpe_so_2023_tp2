#include <commands.h>
#include <libasm.h>
#include <philosophers.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <tests.h>

#define MAX_ARGS     10
#define MAX_COMMANDS 30
#define BUFF_SIZE    64

typedef struct
{
	EntryPoint entry_point;
	int priority;
	char *name, *desc;
} Command;

static void load_command(EntryPoint entry_point, int priority, char* name, char* desc);
static int is_valid_command(char* command);

// commands
static int help(int argc, char** argv);
static int datetime(int argc, char** argv);
static int clear_scr(int argc, char** argv);
static int setcolor(int argc, char** argv);
static int switchcolors(int argc, char** argv);
static int mem(int argc, char** argv);
static int ps(int argc, char** argv);
static int kill(int argc, char** argv);
static int nice(int argc, char** argv);
static int block(int argc, char** argv);
static int loop(int argc, char** argv);
static int cat(int argc, char** argv);
static int wc(int argc, char** argv);
static int filter(int argc, char** argv);
static int philo(int argc, char** argv);
static int testioe(int argc, char** argv);
static int testzde(int argc, char** argv);
static int testmm(int argc, char** argv);
static int testproc(int argc, char** argv);
static int testprio(int argc, char** argv);
static int testsync(int argc, char** argv);
static int testpipe(int argc, char** argv);

static Command commands[MAX_COMMANDS];
static uint16_t commands_len = 0;

static char* args[MAX_ARGS];
static uint8_t args_len;

char buff[BUFF_SIZE];

extern Color color;

void
cmd_init()
{
	load_command(help, 0, "help", "          Displays this help message");
	load_command(datetime, 0, "datetime", "      Prints the current datetime");
	load_command(setcolor, 0, "setcolor", "      Sets foreground, background, prompt, output or error colors");
	load_command(switchcolors, 0, "switchcolors", "  Inverts the background and foreground colors");
	load_command(clear_scr, -20, "clear", "         Clears the screen");
	load_command(mem, -5, "mem", "           Prints the current memory state");
	load_command(ps, -5, "ps", "            Prints a list of all running processes");
	load_command(kill, -5, "kill", "          Kills a process submitting his PID");
	load_command(nice, -5, "nice", "          Changes the priority a processes submitting his PID and new priority");
	load_command(block, -5, "block", "         Toggles the state of a process between BLOCKED and READY");
	load_command(loop, 0, "loop", "          Prints his PID every given amount of seconds");
	load_command(cat, 0, "cat", "           Prints what it reads from STDIN");
	load_command(wc, 0, "wc", "            Counts the amount of lines from a given input");
	load_command(filter, 0, "filter", "        Filters only the vowels from a given input");
	load_command(philo, 0, "philo", "         A 'Dining Philosophers Problem' implementation");
	load_command(testioe, 0, "testioe", "       Tests the 'Invalid Opcode Exception'");
	load_command(testzde, 0, "testzde", "       Tests the 'Zero Division Error Exception'");
	load_command(testmm, 0, "testmm", "        Test the memory manager");
	load_command(testproc, 0, "testproc", "      Test processes manipulation");
	load_command(testprio, 0, "testprio", "      Test the priorities");
	load_command(testsync, 0, "testsync", "      Test semaphores");
	load_command(testpipe, 0, "testpipe", "      Test pipes");
}

int
cmd_execute(char* buf, uint32_t len)
{
	args_len = strtok(buf, ' ', args, MAX_ARGS);
	if (args_len == 0)
		return 0;

	uint8_t is_fg = 1;
	if (strcmp(args[args_len - 1], "&")) {
		is_fg = 0;
		args_len--;
	}

	uint8_t is_piped = 0, cmd1_args = 0, cmd2_args = 0;
	for (int i = 0; i < args_len; i++) {
		if (strcmp(args[i], "|")) {
			is_piped = 1;
			cmd1_args = i;
			break;
		}
	}

	if (is_piped && !is_fg) {
		puts("Cannot use pipes with background processes\n", color.error);
		return -1;
	}

	if (is_piped)
		cmd2_args = args_len - cmd1_args - 1;
	else
		cmd1_args = args_len;

	int cmd1_idx, cmd2_idx;
	if ((cmd1_idx = is_valid_command(args[0])) == -1) {
		puts("Command not found: ", color.output);
		puts(args[0], color.output);
		putchar('\n', color.output);
		return -1;
	}

	if (is_piped) {
		if ((cmd2_idx = is_valid_command(args[cmd1_args + 1])) == -1) {
			puts("Command not found: ", color.output);
			puts(args[cmd1_args + 1], color.output);
			putchar('\n', color.output);
			return -1;
		}
	}

	ProcessCreateInfo create_info1, create_info2;
	create_info1 = (ProcessCreateInfo){
		.name = args[0],
		.argc = cmd1_args - 1,
		.argv = args + 1,
		.entry_point = commands[cmd1_idx].entry_point,
		.is_fg = is_fg,
		.priority = commands[cmd1_idx].priority,
	};

	if (is_piped) {
		create_info2 = (ProcessCreateInfo){
			.name = args[cmd1_args + 1],
			.argc = cmd2_args - 1,
			.argv = args + cmd1_args + 2,
			.entry_point = commands[cmd2_idx].entry_point,
			.is_fg = is_fg,
			.priority = commands[cmd2_idx].priority,
		};
	}

	int pid1, pid2;
	if ((pid1 = asm_execve(&create_info1)) == -1) {
		puts("Error creating process: ", color.output);
		puts((char*)create_info1.name, color.output);
		putchar('\n', color.output);
		return -1;
	}

	if (is_piped && (pid2 = asm_execve(&create_info2)) == -1) {
		asm_kill(pid1);
		puts("Error creating process: ", color.output);
		puts((char*)create_info2.name, color.output);
		putchar('\n', color.output);
		return -1;
	}

	char* pname;
	if (is_piped) {
		int pipes[2];
		char buf[2];
		int_to_str(pid2, buf);
		pname = strcat((char*)create_info1.name, buf);

		asm_pipe_open(pid1, pname ,pipes);
		asm_dup(pid1, pipes[1], STDOUT);
		asm_close(pid1, pipes[0]);
		asm_close(pid1, pipes[1]);
		asm_pipe_open(pid2, pname ,pipes);
		asm_dup(pid2, pipes[0], STDIN);
		asm_close(pid2, pipes[0]);
		asm_close(pid2, pipes[1]);
	}

	int ret1, ret2 = 0;
	if (is_fg) {
		ret1 = asm_waitpid(pid1);
		if (is_piped) {
			ret2 = asm_waitpid(pid2);
			//asm_pipe_unlink(pname);
			asm_free(pname);
		}
		return ret1 || ret2;
	}
	return 0;
}

static void
load_command(EntryPoint entry_point, int priority, char* name, char* desc)
{
	if (commands_len >= MAX_COMMANDS)
		return;
	commands[commands_len].entry_point = entry_point;
	commands[commands_len].priority = priority;
	commands[commands_len].name = name;
	commands[commands_len++].desc = desc;
}

static int
is_valid_command(char* command)
{
	for (int i = 0; i < commands_len; i++) {
		if (strcmp(command, commands[i].name))
			return i;
	}
	return -1;
}

/******************************************/
/**************** COMMANDS ****************/
/******************************************/

static int
help(int argc, char** argv)
{
	for (int i = 0; i < commands_len; i++) {
		puts(commands[i].name, color.output);
		puts(commands[i].desc, color.output);
		putchar('\n', color.output);
	}
	return 0;
}

static int
datetime(int argc, char** argv)
{
	asm_time(color.output);
	return 0;
}

static int
clear_scr(int argc, char** argv)
{
	clear(color.bg);
	return 0;
}

static int
testioe(int argc, char** argv)
{
	asm_testioe();
	return 0;
}

static int
testzde(int argc, char** argv)
{
	asm_testzde();
	return 0;
}

static int
setcolor(int argc, char** argv)
{
	if (args_len != 3) {
		puts("USAGE: setcolor <target> <color>\n<target>   fg | bg | output | prompt | error\n<color>    The color in "
		     "hex format ('0xXXXXXX' or '#XXXXXX')\n",
		     color.output);
		return -1;
	}

	if (!is_hex_color_code(args[2])) {
		puts("Invalid color '", color.output);
		puts(args[2], color.output);
		puts("'\n", color.output);
		return -1;
	}

	char* targets[] = { "fg", "bg", "output", "prompt", "error" };
	uint32_t targets_len = sizeof(targets) / sizeof(targets[0]);

	for (int i = 0; i < targets_len; i++) {
		if (strcmp(targets[i], args[1])) {
			uint32_t col = hex_to_uint(args[2]);
			switch (i) {
				case 0: {
					color.fg = col;
				} break;

				case 1: {
					color.bg = col;
					clear_scr(argc, argv);
				} break;

				case 2: {
					color.output = col;
				} break;

				case 3: {
					color.prompt = col;
				} break;

				case 4: {
					color.error = col;
				} break;
			}
			return 0;
		}
	}

	puts("Invalid target '", color.output);
	puts(args[1], color.output);
	puts("'\n", color.output);
	return -1;
}

static int
switchcolors(int argc, char** argv)
{
	uint32_t aux = color.bg;
	color.bg = color.fg;
	color.fg = aux;
	clear_scr(argc, argv);
	return 0;
}

static int
mem(int argc, char** argv)
{
	asm_meminfo(color.output);
	return 0;
}

static int
ps(int argc, char** argv)
{
	asm_ps(color.output);
	return 0;
}

static int
loop(int argc, char** argv)
{
	if (argc != 0)
		return -1;

	int_to_str(asm_getpid(), buff);

	while (1) {
		asm_sleep(3000);
		puts("Hi, I am loop a process with id = ", color.output);
		puts(buff, color.output);
		putchar('\n', color.output);
	}
	return 0;
}

static int
kill(int argc, char** argv)
{
	if (argc != 1)
		return -1;
	int pid = str_to_int(argv[0]);
	return asm_kill(pid);
}

static int
nice(int argc, char** argv)
{
	if (argc != 2)
		return -1;
	int pid = str_to_int(argv[0]);
	int nice = str_to_int(argv[1]);
	return asm_nice(nice, pid);
}

static int
block(int argc, char** argv)
{
	if (argc != 1)
		return -1;
	int pid = str_to_int(argv[0]);
	return asm_block(pid);
}

static int
cat(int argc, char** argv)
{
	if (argc != 0)
		return -1;

	uint8_t eof;
	int len;
	do {
		len = gets(buff, BUFF_SIZE, &eof, color.fg);
		if (len != 0) {
			puts(buff, color.output);
			putchar('\n', color.output);
		}
	} while (!eof);
	return 0;
}

// TODO: wc cuenta lines no chars
static int
wc(int argc, char** argv)
{
	if (argc != 0)
		return -1;

	uint8_t eof;
	int lines = 0;
	do {
		gets(buff, BUFF_SIZE, &eof, color.fg);
		lines++;
	} while (!eof);
	int_to_str(lines, buff);
	puts(buff, color.output);
	putchar('\n', color.output);
	return 0;
}

static int
filter(int argc, char** argv)
{
	if (argc != 0)
		return -1;

	uint8_t eof;
	int len;
	do {
		len = gets(buff, BUFF_SIZE, &eof, color.fg);
		if (len != 0) {
			for (int i = 0; i < len; i++) {
				char c = buff[i];
				if (c == 'a' || c == 'A' || c == 'e' || c == 'E' || c == 'i' || c == 'I' || c == 'o' || c == 'O' ||
				    c == 'u' || c == 'U' || c == ' ') {
					putchar(c, color.output);
				}
			}
			putchar('\n', color.output);
		}
	} while (!eof);
	return 0;
}

static int
philo(int argc, char** argv)
{
	return init_philo_dilemma(argc, argv);
}

static int
testmm(int argc, char** argv)
{
	test_mm();
	return 0;
}

static int
testproc(int argc, char** argv)
{
	return test_processes(argc, argv);
}

static int
testprio(int argc, char** argv)
{
	return test_prio(argc, argv);
}

static int
testsync(int argc, char** argv)
{
	return test_sync(argc, argv);
}

static int
testpipe(int argc, char** argv)
{
	return test_pipes(argc, argv);
}
