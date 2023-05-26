#define ZERO_EXCEPTION_ID 0
#define INV_OP_EXCEPTION_ID 1
#include <libc.h>
#include <text.h>


void
exception_dispatcher(int exception)
{
	switch (exception) {
		case ZERO_EXCEPTION_ID: {
			exceptionHandler("Zero Division Exception \n");
		} break;
		case INV_OP_EXCEPTION_ID: {
			exceptionHandler("Invalid Operand Exception \n");
		} break;
	}
}

static void
exceptionHandler(uint8_t * msg)
{	
	// REGISTERS ALREADY PRINTED
	tx_put_word(msg);
	
	
}

