#include <exceptions.h>
#include <libc.h>
#include <text.h>
#include <video.h>

#define BUFF_SIZE 30

enum exceptions
{
	ZERO_DIVISION = 0,
	INVALID_OPCODE = 6
};

struct restore_point
{
	uint64_t ip, sp;
};

typedef struct restore_point RestorePoint;

static char* registers[] = {
	"R15: 0x", "R14: 0x", "R13: 0x", "R12: 0x", "R11: 0x", "R10: 0x", "R9:  0x", "R8:  0x", "RSI: 0x",
	"RDI: 0x", "RBP: 0x", "RDX: 0x", "RCX: 0x", "RBX: 0x", "RAX: 0x", "IP:  0x", "RSP: 0x",
};
static uint32_t registers_len = sizeof(registers) / sizeof(registers[0]);
static char buff[BUFF_SIZE];
static RestorePoint rp;

static void exception_handler(char* msg);
static void restore_state(uint64_t* stack);

void
exception_dispatcher(uint32_t exception, uint64_t* stack)
{
	switch (exception) {
		case ZERO_DIVISION: {
			exception_handler("Zero Division Error Exception");
		} break;

		case INVALID_OPCODE: {
			exception_handler("Invalid Opcode Exception");
		} break;
	}
	exc_printreg(stack);
	restore_state(stack);
}

void
exc_printreg(uint64_t* stack)
{
	for (int i = 0; i < registers_len; i++) {
		tx_put_word(registers[i]);
		uint_to_base(stack[i], buff, HEX);
		tx_put_word(buff);
		tx_put_char('\n');
	}

	// solo queda imprimir el rsp
	tx_put_word(registers[registers_len - 1]);
	uint_to_base(stack[registers_len - 1], buff, HEX);
	tx_put_word(buff);
	tx_put_char('\n');
}

void
exc_set_restore_point(uint64_t ip, uint64_t sp)
{
	rp.ip = ip;
	rp.sp = sp;
}

static void
exception_handler(char* msg)
{
	vd_set_color(0xFFFFFF, 0xFF0000);
	tx_put_word(msg);
	vd_set_color(0xFFFFFF, 0x000000);
	tx_put_char('\n');
}

static void
restore_state(uint64_t* stack)
{
	vd_set_color(0xFFFFFF, 0x2020CF);
	tx_put_word("Restoring state from: IP=0x");
	uint_to_base(rp.ip, buff, HEX);
	tx_put_word(buff);
	tx_put_word("  SP=0x");
	uint_to_base(rp.sp, buff, HEX);
	tx_put_word(buff);
	vd_set_color(0xFFFFFF, 0x000000);
	tx_put_word("\n\n");

	stack[registers_len - 2] = rp.ip;
	stack[registers_len + 1] = rp.sp;
}
