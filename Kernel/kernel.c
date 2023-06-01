#include <exceptions.h>
#include <font.h>
#include <idtLoader.h>
#include <interrupts.h>
#include <keyboard.h>
#include <libasm.h>
#include <libc.h>
#include <moduleLoader.h>
#include <sound.h>
#include <stdint.h>
#include <text.h>
#include <time.h>
#include <video.h>

typedef int (*EntryPoint)();

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t end_of_kernel_bin;
extern uint8_t end_of_kernel;

static const uint64_t page_size = 0x1000;
static void* const sample_code_module_addr = (void*)0x400000;
static void* const sample_data_module_addr = (void*)0x500000;

void
clear_bss(void* bss_addr, uint64_t bss_size)
{
	memset(bss_addr, 0, bss_size);
}

void*
get_stack_base()
{
	return (void*)((uint64_t)&end_of_kernel + page_size * 8  // The size of the stack itself, 32KiB
	               - sizeof(uint64_t));                      // Begin at the top of the stack
}

void*
init_kernel_binary()
{
	void* module_addrs[] = {
		sample_code_module_addr,
		sample_data_module_addr,
	};
	load_modules(&end_of_kernel_bin, module_addrs);
	clear_bss(&bss, &end_of_kernel - &bss);
	return get_stack_base();
}

int
main()
{
	idt_loader();

	// print intro wallpaper
	vd_print_wallpaper(2);
	ti_sleep(1 * 18);
	sd_play(900, 0.2 * 18);
	sd_play(500, 0.3 * 18);
	ti_sleep(1 * 18);
	vd_clear();

	exc_set_restore_point((uint64_t)sample_code_module_addr, asm_getsp());
	return ((EntryPoint)sample_code_module_addr)();
}
