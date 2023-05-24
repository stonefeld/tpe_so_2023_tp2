#include <keyboard.h>
#include <syscalls.h>
#include <video.h>

enum SYSCALLS {SYS_READ=4,SYS_WRITE=1, INFO_REG=2};
uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {
		case SYS_READ: {
			return kb_getkey(); // sys_read
		} break;
		case SYS_WRITE: {
			// rsi = char* msg
			// rdx = size
			vd_put_wordn(rsi, rdi);
			return 0;

		}
		case INFO_REG:{

		}
	}
	return 0;
}
