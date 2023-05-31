#include <libasm.h>
#include <pong.h>
#include <shell.h>
#include <stdlib.h>
#include <syscalls.h>

#define MAX_COMMANDS 20
#define MAX_ARGS 3
#define INPUT_SIZE 200

typedef struct
{
	uint32_t (*fn)();
	char *name, *desc;
} Command;

static Command commands[MAX_COMMANDS];
static uint32_t commands_len = 0;
static char* args[MAX_ARGS];
static uint32_t args_len = 0;
static char input_buffer[INPUT_SIZE];
static uint8_t running = 1;
static uint32_t fg = 0xFFFFFF;
static uint32_t bg = 0x000000;

static void load_commands();
static void load_command(uint32_t (*fn)(), char* name, char* desc);
static uint32_t process_input(char* buff, uint32_t size);
static void prompt();

// commands
static uint32_t help();
static uint32_t datetime();
static uint32_t exit();
static uint32_t printreg();
static uint32_t clear();
static uint32_t testioe();
static uint32_t testzde();
static uint32_t pong();
static uint32_t setcolors();

uint32_t
shell_init()
{
	puts("Welcome to the shell!\nStart by typing 'help' on the prompt\n");
	load_commands();

	uint32_t len, status = 0;
	while (running) {
		prompt();
		len = gets(input_buffer, INPUT_SIZE);
		asm_setcolor(fg, bg);
		status = process_input(input_buffer, len);
	}

	return status;
}

static void
load_commands()
{
	load_command(help, "help", "       Displays this help message");
	load_command(datetime, "datetime", "   Prints the current datetime");
	load_command(clear, "clear", "      Clears the screen");
	load_command(exit, "exit", "       Exits the shell");
	load_command(printreg, "printreg", "   Prints all the registers values");
	load_command(testioe, "testioe", "    Tests the 'Invalid Opcode Exception'");
	load_command(testzde, "testzde", "    Tests the 'Zero Division Error Exception'");
	load_command(pong, "pong", "       Pong (The Game)");
	load_command(setcolors, "setcolors", "  Sets background and foreground colors received in format '0xXXXXXX'");
}

static void
load_command(uint32_t (*fn)(), char* name, char* desc)
{
	if (commands_len >= MAX_COMMANDS)
		return;
	commands[commands_len].fn = fn;
	commands[commands_len].name = name;
	commands[commands_len].desc = desc;
	commands_len++;
}

static uint32_t
process_input(char* buff, uint32_t size)
{
	args_len = strtok(buff, ' ', args, MAX_ARGS);
	if (args_len <= 0 || args_len > MAX_ARGS) {
		puts("Wrong ammount of arguments\n");
		return -1;
	}
	for (int i = 0; i < commands_len; i++) {
		if (strcmp(args[0], commands[i].name))
			return commands[i].fn();
	}
	puts("Command not found: ");
	puts(args[0]);
	putchar('\n');
	return -1;
}

static void
prompt()
{
	asm_setcolor(0x00ff00, bg);
	puts("user@qemu");
	asm_setcolor(fg, bg);
	putchar(':');
	putchar('~');
	puts("$ ");
}

static uint32_t
help()
{
	for (int i = 0; i < commands_len; i++) {
		puts(commands[i].name);
		puts(commands[i].desc);
		putchar('\n');
	}
	return 0;
}

static uint32_t
datetime()
{
	asm_datetime();
	return 0;
}

static uint32_t
clear()
{
	asm_clear();
	return 0;
}

static uint32_t
exit()
{
	running = 0;
	return 0;
}

static uint32_t
printreg()
{
	asm_printreg();
	return 0;
}

static uint32_t
testioe()
{
	asm_testioe();
	return 0;
}

static uint32_t
testzde()
{
	asm_testzde();
	return 0;
}

static uint32_t
pong()
{
	uint8_t ret = start_game();
	asm_setcolor(0xFFFFFF, 0x000000);
	asm_clear();
	return ret;
}
static uint32_t
setcolors()
{
	if (args_len != 3 || !is_hex_color_code(args[1]) || !is_hex_color_code(args[2])) {
		puts("Invalid color codes \n");
		return 0;
	}
	bg = hex_to_uint(args[1]);
	fg = hex_to_uint(args[2]);
	asm_setcolor(fg, bg);
	clear();
	return 0;
}
