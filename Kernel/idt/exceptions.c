#define ZERO_EXCEPTION_ID 0
#define INV_OP_EXCEPTION_ID 1

static void zero_division();
static void inv_operator();

void
exception_dispatcher(int exception)
{
	switch (exception) {
		case ZERO_EXCEPTION_ID: {
			zero_division();
		} break;
		case INV_OP_EXCEPTION_ID: {
			inv_operator();
		} break;
	}
}

static void
zero_division()
{
	// Handler para manejar excepcíon
}

static void
inv_operator()
{
	// Handler para manejar excepcion
}