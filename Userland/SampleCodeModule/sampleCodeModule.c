#include <shell.h>
#include <stdio.h>
#include <syscalls.h>

int
main()
{
	uint32_t status = shell_init();
	puts("\nShell finished executing.\n", WHITE);
	return status;
}
