#include "keyboard.h"

#include <lib.h>
#include <naiveConsole.h>

static const char scanCodes[256] = {
	0,    0,    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
	'\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[',  ']',
	'\n', 0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
	0,    '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', 0,   0,
};

void
printChar()
{
	char key = getKey();

	if (key == 0x39) {  // space
		ncPrint(" ");
		return;
	}
	if (scanCodes[key] == '\b') {
		ncBackspace();
		return;
	}
	if (scanCodes[key] == '\t') {
		ncPrint("    ");
		return;
	}
	if (scanCodes[key] == '\n') {
		ncNewline();
		return;
	}
	if (key >= 0 && key <= 256 && scanCodes[key] != 0) {
		ncPrintChar(scanCodes[key]);
		return;
	}
}
