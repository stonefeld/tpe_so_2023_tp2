#include <libc.h>
#include <text.h>
#include <libasm.h>
#include <interrupts.h>
#define ZERO_EXCEPTION_ID 0
#define INV_OP_EXCEPTION_ID 1

static void exception_handler(uint8_t* msg);

void
exception_dispatcher(int exception)
{
	switch (exception) {
		case ZERO_EXCEPTION_ID: {

			exception_handler((uint8_t*)" Zero Division Exception\n");
		} break;

		case INV_OP_EXCEPTION_ID: {
			exception_handler((uint8_t*)"Invalid Operand Exception\n");
		} break;
	}
}

static void
exception_handler(uint8_t* msg)
{
	// REGISTERS ALREADY PRINTED
	tx_put_word(msg);
	tx_put_word("\n Presione enter para continuar: ");
	int c;
	do{
		asm_hlt();		// halt
	} while((c=kb_getkey()) != '\n');
	tx_clear();
	give_control_to_user();
}
