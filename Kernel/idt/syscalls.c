#include <font.h>
#include <keyboard.h>
#include <libasm.h>
#include <rtc.h>
#include <sound.h>
#include <syscalls.h>
#include <text.h>
#include <time.h>
#include <video.h>

enum syscalls
{
	// i/o interaction
	SYS_READ = 1,
	SYS_WRITE,

	// drawing
	SYS_DRAW,
	SYS_CLEAR,
	SYS_COLOR,
	SYS_CURSOR,
	SYS_SHOW_CURSOR,

	// properties
	SYS_WINWIDTH,
	SYS_WINHEIGHT,
	SYS_FONTWIDTH,
	SYS_FONTHEIGHT,

	// system
	SYS_TICKS,
	SYS_SLEEP,
	SYS_REGS,
	SYS_RTC,
	SYS_SOUND
};

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {
		case SYS_READ: {
			return kb_getchar((uint8_t*)rsi);
		} break;

		case SYS_WRITE: {
			tx_put_char(rsi, rdx);
		} break;

		case SYS_DRAW: {
			vd_draw(rsi, rdx, rcx, r8, r9);
		} break;

		case SYS_CLEAR: {
			tx_clear(rsi);
		} break;

		case SYS_CURSOR: {
			tx_set_cursor(rsi, rdx, rcx);
		} break;

		case SYS_SHOW_CURSOR: {
			tx_show_cursor(rsi);
		} break;

		case SYS_WINWIDTH: {
			return vd_get_winwidth();
		} break;

		case SYS_WINHEIGHT: {
			return vd_get_winheight();
		} break;

		case SYS_FONTWIDTH: {
			return CHAR_WIDTH;
		} break;

		case SYS_FONTHEIGHT: {
			return CHAR_HEIGHT;
		} break;

		case SYS_TICKS: {
			return ti_ticked();
		} break;

		case SYS_SLEEP: {
			ti_sleep(rsi);
		} break;

		case SYS_REGS: {
			asm_printreg(rsi);
		} break;

		case SYS_RTC: {
			rtc_datetime(rsi);
		} break;

		case SYS_SOUND: {
			sd_play(rsi, rdx);
		} break;
	}
	return 0;
}
