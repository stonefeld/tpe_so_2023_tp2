#include <shell.h>
#include <stdlib.h>
#include <syscallHandler.h>

#define MAX_COMMANDS 20
#define INPUT_SIZE 36

struct command
{
	uint32_t (*fn)();
	uint8_t *name, *desc;
};

typedef struct command Command;

static Command commands[MAX_COMMANDS];
static uint32_t commands_len = 0;
static uint8_t input_buffer[INPUT_SIZE];
static uint8_t running = 1;

static void load_commands();
static void load_command(uint32_t (*fn)(), uint8_t* name, uint8_t* desc);
static uint32_t process_input(uint8_t* buff, uint32_t size);

// commands
static uint32_t help();
static uint32_t time();
static uint32_t clear();
static uint32_t exit();


uint32_t
shell_init()
{
	load_commands();

	uint32_t len, status = 0;

	while (running) {
		len = gets((uint8_t*)input_buffer, INPUT_SIZE);
		status = process_input((uint8_t*)input_buffer, len);
	}

	return status;
}

static void
load_commands()
{
	load_command(help, (uint8_t*)"help", (uint8_t*)"Displays this help message");
	load_command(time, (uint8_t*)"time", (uint8_t*)"");
	load_command(clear, (uint8_t*)"clear", (uint8_t*)"");
	load_command(exit, (uint8_t*)"exit", (uint8_t*)"");
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
	for (int i = 0; i < commands_len; i++) {
		if (strcmp(buff, commands[i].name)) {
			return commands[i].fn();
		}
	}
	return -1;
}

static uint32_t
help()
{
	puts((uint8_t*)"Este es un mensaje de ayuda\n");
	return 0;
}

static uint32_t
time()
{
	asm_time();
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
