#include <keyboard.h>
#include <syscalls.h>
#include <text.h>
#include <video.h>

enum SYSCALLS
{
	SYS_WRITE_ID = 1,
	SYS_REGS_ID,
	SYS_READ_ID = 4,
	SYS_RTC_ID,			// Real Time Clock
	SYS_CLEAR_ID,
	SYS_EXIT_ID
};

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {
		case SYS_WRITE_ID: {
			tx_put_char(rsi);
		} break;

		case SYS_REGS_ID: {
		} break;

		case SYS_READ_ID: {
			return kb_getkey();
		} break;

		case SYS_RTC_ID: {
		} break;

		case SYS_CLEAR_ID: {
		} break;

		case SYS_EXIT_ID: {
		} break;
	}
	return 0;
}
