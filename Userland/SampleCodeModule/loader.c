#include <stdint.h>
#include <stdlib.h>

extern char bss;
extern char endOfBinary;

extern int main();

int
_start()
{
	// Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);
	return main();
}
