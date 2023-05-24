#include <keyboard.h>
#include <syscalls.h>

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {
		case 4: {
			return kb_getkey();
		} break;
	}
	return 0;
}
