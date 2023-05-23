
#define ZERO_EXCEPTION_ID 0

static void zero_division();

void
exceptionDispatcher(int exception)
{
	switch (exception) {
		case ZERO_EXCEPTION_ID:
			zero_division();
			break;

		// ...

		default:
			break;
	}
}

static void
zero_division()
{
	// Handler para manejar excepcíon
}