#include <keyboard.h>
#include <syscalls.h>
#include <text.h>
#include <video.h>

enum SYSCALLS
{
	SYS_WRITE = 1,
	INFO_REG,
	SYS_READ = 4
};

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {
		case SYS_WRITE: {
			tx_put_char(rsi);
		} break;

		case INFO_REG: {
		} break;

		case SYS_READ: {
			return kb_getkey();
		} break;
	}
	return 0;
}
