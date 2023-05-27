#include <keyboard.h>
#include <libasm.h>
#include <rtc.h>
#include <syscalls.h>
#include <text.h>
#include <time.h>
#include <video.h>

enum syscalls
{
	SYS_READ = 1,
	SYS_WRITE,
	SYS_REGS,
	SYS_CLEAR,
	SYS_RTC,
	SYS_COLOR,
	SYS_DRAW,
	SYS_WINWIDTH,
	SYS_WINHEIGHT,
	SYS_TICKS,
	SYS_CURSOR,
	SYS_RELEASED
};

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {
		case SYS_READ: {
			return kb_getpressed();
		} break;

		case SYS_WRITE: {
			tx_put_char(rsi);
		} break;

		case SYS_REGS: {
			asm_printreg();
		} break;

		case SYS_CLEAR: {
			tx_clear();
		} break;

		case SYS_RTC: {
			rtc_datetime();
		} break;

		case SYS_COLOR: {
			vd_set_color(rsi, rdx);
		} break;

		case SYS_DRAW: {
			vd_draw_figure(rsi, rdx, rcx, r8);
		} break;

		case SYS_WINWIDTH: {
			return vd_get_winwidth();
		} break;

		case SYS_WINHEIGHT: {
			return vd_get_winheight();
		} break;

		case SYS_TICKS: {
			return ticks_elapsed();
		} break;

		case SYS_CURSOR: {
			tx_set_cursor(rsi, rdx);
		} break;

		case SYS_RELEASED: {
			return kb_getreleased();
		} break;
	}
	return 0;
}
