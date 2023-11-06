#include <commands.h>
#include <libasm.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <test_mm.h>
#include <test_processes.h>

#define MAX_ARGS 10
#define MAX_COMMANDS 30

typedef struct
{
	EntryPoint entry_point;
	char *name, *desc;
} Command;

static void load_command(EntryPoint entry_point, char* name, char* desc);

// commands
static int help(int argc, char** argv);
static int datetime(int argc, char** argv);
static int exit(int argc, char** argv);
static int clear_scr(int argc, char** argv);
static int setcolor(int argc, char** argv);
static int switchcolors(int argc, char** argv);
static int mem(int argc, char** argv);
static int ps(int argc, char** argv);
static int loop(int argc, char** argv);
static int kill(int argc, char** argv);
static int nice(int argc, char** argv);
static int block(int argc, char** argv);
static int cat(int argc, char** argv);
static int wc(int argc, char** argv);
static int filter(int argc, char** argv);
static int phylo(int argc, char** argv);
static int testioe(int argc, char** argv);
static int testzde(int argc, char** argv);
static int testmm(int argc, char** argv);
static int testproc(int argc, char** argv);

static Command commands[MAX_COMMANDS];
static uint16_t commands_len = 0;

static char* args[MAX_ARGS];
static uint8_t args_len;

extern Color color;

void
cmd_init()
{
	// TODO: revisar idiomas
	load_command(help, "help", "          Displays this help message");
	load_command(datetime, "datetime", "      Prints the current datetime");
	load_command(setcolor, "setcolor", "      Sets foreground, background, prompt, output or error colors");
	load_command(switchcolors, "switchcolors", "  Inverts the background and foreground colors");
	load_command(clear_scr, "clear", "         Clears the screen");
	load_command(mem, "mem", "           Imprime estado de la memoria");
	load_command(ps, "ps", "            Imprime la lista de todos los procesos con sus propiedades");
	load_command(loop, "loop", "          Imprime su ID con un saludo cada una determinada cantidad de segundos");
	load_command(kill, "kill", "          Mata un proceso dado su ID");
	load_command(nice, "nice", "          Cambia la prioridad de un proceso dado su ID y la nueva prioridad");
	load_command(block, "block", "         Cambia el estado de un proceso entre bloqueado y listo dado su ID");
	load_command(cat, "cat", "           Imprime el stdin tal como lo recibe");
	load_command(wc, "wc", "            Cuenta la cantidad de lineas del input");
	load_command(filter, "filter", "        Filtra las vocales del input");
	load_command(phylo, "phylo", "         Implementa el problema de los filosofos comensales");
	load_command(testioe, "testioe", "       Tests the 'Invalid Opcode Exception'");
	load_command(testzde, "testzde", "       Tests the 'Zero Division Error Exception'");
	load_command(testmm, "testmm", "        Test memory manager");
	load_command(testproc, "testproc", "      Test processes");
	load_command(exit, "exit", "          Exits the shell");
}

int
cmd_execute(char* buf, uint32_t len)
{
	args_len = strtok(buf, ' ', args, MAX_ARGS);
	if (args_len == 0)
		return -1;

	uint8_t is_fg = 1;
	if (strcmp(args[args_len - 1], "&")) {
		is_fg = 0;
		args_len--;
	}

	for (int i = 0; i < commands_len; i++) {
		if (strcmp(args[0], commands[i].name)) {
			ProcessCreateInfo create_info = {
				.name = args[0],
				.argc = args_len - 1,
				.argv = args + 1,
				.entry_point = commands[i].entry_point,
				.is_fg = is_fg,
				.priority = 0,
			};
			int pid = asm_execve(&create_info);
			return is_fg ? asm_waitpid(pid) : 0;
		}
	}
	puts("Command not found: ", color.output);
	puts(args[0], color.output);
	putchar('\n', color.output);
	return -1;
}

static void
load_command(EntryPoint entry_point, char* name, char* desc)
{
	if (commands_len >= MAX_COMMANDS)
		return;
	commands[commands_len].entry_point = entry_point;
	commands[commands_len].name = name;
	commands[commands_len++].desc = desc;
}

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
exit(int argc, char** argv)
{
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
	return 0;
}

static int
block(int argc, char** argv)
{
	return 0;
}

static int
cat(int argc, char** argv)
{
	return 0;
}

static int
wc(int argc, char** argv)
{
	return 0;
}

static int
filter(int argc, char** argv)
{
	return 0;
}

static int
phylo(int argc, char** argv)
{
	return 0;
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
