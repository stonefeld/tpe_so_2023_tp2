#include <keyboardDriver.h>
#include <syscalls.h>

int
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r9, uint64_t r8)
{
	// alguno de nuestros parámetros funcionara como ID
	switch (r8)  // estaríamos usando r8 en este caso
	{
		case 4: {
			// sys_read();
		} break;
	}
}

int
sys_read(uint64_t fd, char* buffer, uint64_t size)
{
	// Acá implementamos todas nuestras syscalls en C.
	return 0;
}
