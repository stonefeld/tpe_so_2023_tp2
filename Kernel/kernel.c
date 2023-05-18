#include "videoDriver.h"

#include <keyboard.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <string.h>

#define RENGLON_HEIGHT 32

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
	return (void*)((uint64_t)&end_of_kernel +
	               page_size * 8       // The size of the stack itself, 32KiB
	               - sizeof(uint64_t)  // Begin at the top of the stack
	);
}

void*
init_kernel_binary()
{
	char buffer[10];

	nc_print("[x64BareBones]");
	nc_newline();

	nc_print("CPU Vendor: ");
	nc_print(cpu_vendor(buffer));
	nc_newline();

	nc_print("[Loading modules]");
	nc_newline();

	void* module_addrs[] = {
		sample_code_module_addr,
		sample_data_module_addr,
	};
	load_modules(&end_of_kernel_bin, module_addrs);

	nc_print("[Done]");
	nc_newline();
	nc_newline();

	nc_print("[Initializing kernel's binary]");
	nc_newline();

	clear_bss(&bss, &end_of_kernel - &bss);

	nc_print("  text: 0x");
	nc_print_hex((uint64_t)&text);
	nc_newline();
	nc_print("  rodata: 0x");
	nc_print_hex((uint64_t)&rodata);
	nc_newline();
	nc_print("  data: 0x");
	nc_print_hex((uint64_t)&data);
	nc_newline();
	nc_print("  bss: 0x");
	nc_print_hex((uint64_t)&bss);
	nc_newline();

	nc_print("[Done]");
	nc_newline();
	nc_newline();
	return get_stack_base();
}

int
main()
{
	/*
	ncPrint("[Kernel Main]");
	ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	ncPrint("  Calling the sample code module returned: ");
	ncPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	ncNewline();
	ncNewline();

	ncPrint("  Sample data module at 0x");
	ncPrintHex((uint64_t)sampleDataModuleAddress);
	ncNewline();
	ncPrint("  Sample data module contents: ");
	ncPrint((char*)sampleDataModuleAddress);
	ncNewline();

	ncPrint("[Finished]");
	ncNewline();
	ncNewline();

	// ejercicio 2
	ncSetColor(0x2, 0xF);
	ncPrint("Arquitectura de las Computadoras");
	ncClearColor();
	ncNewline();

	// ejercicio 3
	ncPrintTime();

	// ejercicio 4
	// ncClear();
	// int i, teclahex = 0, teclahexant = teclahex;
	// for (int j = 1; j < 1000; j++) {
	// 	i = getKey();
	// 	teclahex = i;
	// 	if (teclahex != teclahexant) {
	// 		ncPrintHex(teclahex);
	// 		ncPrint(" ");
	// 		teclahexant = teclahex;
	// 	}
	// }
	for (int j = 1; j < 1000; j++)
	    printChar();
	*/

	for (int i = 0; i < vbe_mode_info->width; i++) {
		for (int j = 0; j < RENGLON_HEIGHT; j++) {
			vd_put_pixel_rgb(255, 0, 0, i, j);
		}
	}
	for (int i = 0; i < vbe_mode_info->width; i++) {
		for (int j = 0; j < RENGLON_HEIGHT; j++) {
			vd_put_pixel_rgb(255, 128, 0, i, RENGLON_HEIGHT + j);
		}
	}
	for (int i = 0; i < vbe_mode_info->width; i++) {
		for (int j = 0; j < RENGLON_HEIGHT; j++) {
			vd_put_pixel_rgb(255, 255, 0, i, RENGLON_HEIGHT * 2 + j);
		}
	}
	for (int i = 0; i < vbe_mode_info->width; i++) {
		for (int j = 0; j < RENGLON_HEIGHT; j++) {
			vd_put_pixel_rgb(0, 255, 0, i, RENGLON_HEIGHT * 3 + j);
		}
	}
	for (int i = 0; i < vbe_mode_info->width; i++) {
		for (int j = 0; j < RENGLON_HEIGHT; j++) {
			vd_put_pixel_rgb(0, 255, 255, i, RENGLON_HEIGHT * 4 + j);
		}
	}
	for (int i = 0; i < vbe_mode_info->width; i++) {
		for (int j = 0; j < RENGLON_HEIGHT; j++) {
			vd_put_pixel_rgb(0, 0, 255, i, RENGLON_HEIGHT * 5 + j);
		}
	}
	for (int i = 0; i < vbe_mode_info->width; i++) {
		for (int j = 0; j < RENGLON_HEIGHT; j++) {
			vd_put_pixel_rgb(153, 51, 255, i, RENGLON_HEIGHT * 6 + j);
		}
	}
	for (int i = 0; i < vbe_mode_info->width; i++) {
		for (int j = 0; j < RENGLON_HEIGHT; j++) {
			vd_put_pixel_rgb(153, 0, 0, i, RENGLON_HEIGHT * 7 + j);
		}
	}

	for (int i = 0; i < vbe_mode_info->width; i++)
		vd_put_pixel(0xFFFFFF, i, RENGLON_HEIGHT * 8 + 32);
	return 0;
}
