#define ZERO_EXCEPTION_ID 0
#define INV_OP_EXCEPTION_ID 1
#include <libc.h>
#include <text.h>

static void exceptionHandler(uint8_t * msg);

void
exception_dispatcher(int exception)
{
	switch (exception) {
		case ZERO_EXCEPTION_ID: {
			exceptionHandler("\n Zero Division Exception \n");
		} break;
		case INV_OP_EXCEPTION_ID: {
			exceptionHandler("\n Invalid Operand Exception \n");
		} break;
	}
}

static void
exceptionHandler(uint8_t * msg)
{	
	// REGISTERS ALREADY PRINTED
	tx_put_word(msg);
	
	
}

