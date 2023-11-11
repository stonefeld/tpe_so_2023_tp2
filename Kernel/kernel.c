#include <exceptions.h>
#include <font.h>
#include <idtLoader.h>
#include <interrupts.h>
#include <keyboard.h>
#include <libasm.h>
#include <libc.h>
#include <memoryManager.h>
#include <moduleLoader.h>
#include <process.h>
#include <scheduler.h>
#include <sound.h>
#include <stdint.h>
#include <stdlib.h>
#include <text.h>
#include <time.h>
#include <video.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t end_of_kernel_bin;
extern uint8_t end_of_kernel;

static const uint64_t page_size = 0x1000;
static void* const sample_code_module_addr = (void*)0x400000;
static void* const sample_data_module_addr = (void*)0x500000;

static void* const heap_start_addr = (void*)0xF00000;
static void* const heap_end_addr = (void*)0x2000000;

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

void
init_shell()
{
	ProcessCreateInfo info = {
		.name = "shell",
		.entry_point = (ProcessEntryPoint)sample_code_module_addr,
		.is_fg = 1,
		.priority = MAX_PRIORITY,
		.argc = 0,
		.argv = NULL,
	};
	int pid = proc_create(&info);
	kb_map_fd(pid, STDIN);
	tx_map_fd(pid, STDOUT);
	tx_map_fd(pid, STDERR);
}

int
main()
{
	// dehabilito las interrupciones
	asm_cli();

	idt_loader();
	mm_init(heap_start_addr, (heap_end_addr - heap_start_addr));
	sch_init();
	kb_init();
	tx_init();

	// inicializamos la shell
	init_shell();

	// vuelvo a habilitar las interrupciones
	asm_sti();

	while (1) {
		// el proceso kernel no debe hacer mas nada
		sch_yield();
		asm_hlt();
	}

	return 0;
}
