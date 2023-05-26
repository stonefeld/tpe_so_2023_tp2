#include <shell.h>
#include <stdlib.h>
#include <syscallHandler.h>

#define MAX_COMMANDS 20
#define MAX_ARGS 3
#define INPUT_SIZE 36

struct command
{
	uint32_t (*fn)();
	uint8_t *name, *desc;
};

typedef struct command Command;

static Command commands[MAX_COMMANDS];
static uint32_t commands_len = 0;
static uint8_t* args[MAX_ARGS];
static uint8_t input_buffer[INPUT_SIZE];
static uint8_t running = 1;

static void load_commands();
static void load_command(uint32_t (*fn)(), uint8_t* name, uint8_t* desc);
static uint32_t process_input(uint8_t* buff, uint32_t size);
static void prompt();

// commands
static uint32_t help();
static uint32_t datetime();
static uint32_t clear();
static uint32_t exit();

uint32_t
shell_init()
{
	load_commands();

	uint32_t len, status = 0;

	while (running) {
		prompt();
		len = gets(input_buffer, INPUT_SIZE);
		asm_setcolor(0xA8A8A8, 0x000000);
		process_input(input_buffer, len);
	}

	return status;
}

static void
load_commands()
{
	load_command(help, (uint8_t*)"help", (uint8_t*)"Displays this help message\n");
	load_command(datetime, (uint8_t*)"datetime", (uint8_t*)"Prints the current datetime\n");
	load_command(clear, (uint8_t*)"clear", (uint8_t*)"Clears the screen\n");
	load_command(exit, (uint8_t*)"exit", (uint8_t*)"Exits the shell\n");
}

static void
load_command(uint32_t (*fn)(), uint8_t* name, uint8_t* desc)
{
	if (commands_len >= MAX_COMMANDS)
		return;
	commands[commands_len].fn = fn;
	commands[commands_len].name = name;
	commands[commands_len].desc = desc;
	commands_len++;
}

static uint32_t
process_input(uint8_t* buff, uint32_t size)
{
	int args_len = strtok(buff, ' ', args, MAX_ARGS);
	if (args_len <= 0 || args_len > MAX_ARGS) {
		puts((uint8_t*)"Wrong ammount of arguments\n");
		return -1;
	}
	for (int i = 0; i < commands_len; i++) {
		if (strcmp(args[0], commands[i].name))
			return commands[i].fn();
	}
	puts((uint8_t*)"Command '");
	puts(args[0]);
	puts((uint8_t*)"' is not valid.\n\n");
	help();
	return -1;
}

static void
prompt()
{
	asm_setcolor(0x00ff00, 0x000000);
	puts((uint8_t*)"user@qemu");
	asm_setcolor(0xffffff, 0x000000);
	putchar(':');
	asm_setcolor(0x0000ff, 0x000000);
	putchar('~');
	asm_setcolor(0xffffff, 0x000000);
	puts((uint8_t*)"$ ");
}

static uint32_t
help()
{
	puts((uint8_t*)"This are all the available commands:\n");
	for (int i = 0; i < commands_len; i++) {
		putchar('\t');
		puts(commands[i].name);
		puts((uint8_t*)": ");
		puts(commands[i].desc);
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
exit()
{
	running = 0;
	return 0;
}

static uint32_t
clear()
{
	asm_clear();
	return 0;
}
