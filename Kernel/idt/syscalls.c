#include <keyboard.h>
#include <syscalls.h>
#include <text.h>
#include <video.h>

enum SYSCALLS
{
	SYS_EXIT = 0,
	SYS_READ,
	SYS_WRITE,
	SYS_REGS,
	SYS_CLEAR,
	SYS_RTC,
	SYS_COLOR,
	SYS_DRAW,
	SYS_MINWIDTH,
	SYS_MINHEIGHT,

};

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {

		case SYS_EXIT: {
			
		} break;
		
		case SYS_READ: {
			return kb_getkey();
			
		} break;

		case SYS_WRITE: {
			tx_put_char(rsi);
		} break;

		case SYS_REGS: {
			

		} break;
		
		case SYS_CLEAR: {

		} break;
		case SYS_RTC: {

		} break;
		case SYS_COLOR: {

		} break;
		case SYS_DRAW: {

		} break;
		case SYS_MINWIDTH: {

		} break;

		case SYS_MINHEIGHT: {

		} break;
	}
	return 0;
}
