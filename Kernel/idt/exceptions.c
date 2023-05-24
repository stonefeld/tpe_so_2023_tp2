#define ZERO_EXCEPTION_ID 0

static void zero_division();

void
exception_dispatcher(int exception)
{
	switch (exception) {
		case ZERO_EXCEPTION_ID: {
			zero_division();
		} break;
	}
}

static void
zero_division()
{
	// Handler para manejar excepcíon
}
