#include <commands.h>
#include <shell.h>
#include <stdio.h>
#include <syscalls.h>

#define INPUT_SIZE 200

static void prompt(int32_t status);

static char input_buffer[INPUT_SIZE];
static uint8_t running = 1;

// colors
Color color = { .fg = 0xffffff, .bg = 0x000000, .output = 0xa0a0a0, .prompt = 0x00ff00, .error = 0xff0000 };

uint32_t
shell_init()
{
	puts("Welcome to the shell!\nStart by typing 'help' on the prompt\n", color.output);
	cmd_init();
	running = 1;

	int32_t len, status = 0;
	while (running) {
		prompt(status);
		len = gets(input_buffer, INPUT_SIZE, color.fg);
		status = cmd_execute(input_buffer, len);
	}

	return status;
}

static void
prompt(int32_t status)
{
	puts(">>>", status == 0 ? color.prompt : color.error);
	putchar(' ', color.fg);
}
