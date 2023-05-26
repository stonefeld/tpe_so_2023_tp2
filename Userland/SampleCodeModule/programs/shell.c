#include <shell.h>
#include <stdlib.h>

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
static uint8_t* input_buffer[INPUT_SIZE];

static void load_commands();
static void load_command(uint32_t (*fn)(), uint8_t* name, uint8_t* desc);
static uint32_t process_input(uint8_t* buff, uint32_t size);

// commands
static uint32_t help();

uint32_t
shell_init()
{
	load_commands();

	uint8_t error = 0;
	uint32_t len;

	while (!error) {
		len = gets((uint8_t*)input_buffer, INPUT_SIZE);
		process_input((uint8_t*)input_buffer, len);
	}

	return error;
}

static void
load_commands()
{
	load_command(help, (uint8_t*)"help", (uint8_t*)"Displays this help message");
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
