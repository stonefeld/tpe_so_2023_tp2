#include "keyboard.h"

#include <lib.h>
#include <naiveConsole.h>

static const char scancodes[256] = {
	0,    0,    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
	'\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[',  ']',
	'\n', 0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
	0,    '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', 0,   0,
};

void
kb_printkey()
{
	char key = getkey();

	if (key == 0x39) {  // space
		nc_print(" ");
		return;
	}
	if (scancodes[key] == '\b') {
		nc_backspace();
		return;
	}
	if (scancodes[key] == '\t') {
		nc_print("    ");
		return;
	}
	if (scancodes[key] == '\n') {
		nc_newline();
		return;
	}
	if (key >= 0 && key <= 256 && scancodes[key] != 0) {
		nc_putchar(scancodes[key]);
		return;
	}
}
