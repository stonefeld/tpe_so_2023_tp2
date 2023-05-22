// Acá Implementamos las wrappers, funciones etc para proporcionar syscalls
#include <syscalls.h>

static int sys_read(uint64_t fd, char* buffer, uint64_t size);

int
syscall_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r9, uint64_t r8)
{
	//  alguno de nuestros parámetros funcionara como ID
	switch (r8)  // estaríamos usando r8 en este caso
	{
		case 0: {
			// acá llamamos a la wrapper de sys_read x ej:
			// return sys_read(rdi, rsi, rdx);
		} break;

		default: {
		} break;
	}
}

static int
sys_read(uint64_t fd, char* buffer, uint64_t size)
{
	// Acá implementamos todas nuestras syscalls en C.
	return 0;
}
