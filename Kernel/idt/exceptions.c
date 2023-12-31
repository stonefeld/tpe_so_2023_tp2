#include <exceptions.h>
#include <interrupts.h>
#include <keyboard.h>
#include <libc.h>
#include <process.h>
#include <scheduler.h>
#include <syscalls.h>
#include <text.h>
#include <video.h>

#define BUFF_SIZE 30

enum exceptions
{
	ZERO_DIVISION = 0,
	INVALID_OPCODE = 6
};

static char* regs_descriptor[] = {
	"R15: 0x", "R14: 0x", "R13: 0x", "R12: 0x", "R11: 0x", "R10: 0x", "R9:  0x", "R8:  0x", "RSI: 0x",
	"RDI: 0x", "RBP: 0x", "RDX: 0x", "RCX: 0x", "RBX: 0x", "RAX: 0x", "IP:  0x", "RSP: 0x",
};
static uint32_t registers_len = sizeof(regs_descriptor) / sizeof(regs_descriptor[0]);
static char buf[BUFF_SIZE];

static void exception_handler(int pid, char* msg);
static void printreg(uint64_t* stack);
static void fill_and_print(uint64_t data);

extern void init_shell();

void
exception_dispatcher(uint32_t exception, uint64_t* stack)
{
	int pid = sch_get_current_pid();
	switch (exception) {
		case ZERO_DIVISION: {
			exception_handler(pid, "Zero Division Error Exception");
		} break;

		case INVALID_OPCODE: {
			exception_handler(pid, "Invalid Opcode Exception");
		} break;
	}
	printreg(stack);

	proc_kill(pid, 1);
	if (pid == 0)
		init_shell();
	sch_yield();
}

static void
exception_handler(int pid, char* msg)
{
	tx_put_word("PID ", RED);
	uint_to_base(pid, buf, DEC);
	tx_put_word(buf, RED);
	tx_put_word(" crashed. Exception: '", RED);
	tx_put_word(msg, RED);
	tx_put_word("'\n", RED);
}

static void
printreg(uint64_t* stack)
{
	if (!stack)
		return;

	for (int i = 0; i < registers_len - 1; i++) {
		tx_put_word(regs_descriptor[i], RED);
		fill_and_print(stack[i]);
		tx_put_char('\n', RED);
	}
	tx_put_word(regs_descriptor[registers_len - 1], RED);
	fill_and_print(stack[registers_len + 1]);
	tx_put_char('\n', RED);
}

static void
fill_and_print(uint64_t data)
{
	uint32_t len = uint_to_base(data, buf, HEX);
	for (int i = 0; i < 16 - len; i++)
		tx_put_char('0', RED);
	tx_put_word(buf, RED);
}
