#include <defs.h>
#include <idtLoader.h>
#include <interrupts.h>
#include <stdint.h>

#pragma pack(push)  // pushea la alineación actual
#pragma pack(1)     // alinea todas las estructuras a un byte

// descriptor de una interrupcion
struct interrupt_descriptor_64
{
	uint16_t offset_l, selector;
	uint8_t cero, access;
	uint16_t offset_m;
	uint32_t offset_h, other_cero;
};

#pragma pack(pop)  // reestablece la alineación previa

typedef struct interrupt_descriptor_64* IDTEntry;

IDTEntry idt = (IDTEntry)0x0;  // IDT de 255 entradas

static void setup_idt_entry(int index, uint64_t offset);

void
load_idt()
{
	_cli();

	// irqs
	setup_idt_entry(0x20, (uint64_t)&_irq00Handler);
	// in -> Bootloader/Pure64/src/interrupt.asm:
	// kbd interrupt. IRQ 0x01, INT 0x21
	// runs whenever there's input on the kbd
	setup_idt_entry(0x21, (uint64_t)&_irq01Handler);

	// exceptions
	setup_idt_entry(0x00, (uint64_t)&_exception0Handler);
	// setup_idt_entry(0x00, (uint64_t)&_exception1Handler);

	// syscall handler
	setup_idt_entry(0x69, (uint64_t)&_sysCallHandler);

	// Solo interrupcion timer tick habilitadas
	_picMasterMask(0xFC);
	_picSlaveMask(0xFF);

	_sti();
}

static void
setup_idt_entry(int index, uint64_t offset)
{
	idt[index].selector = 0x08;
	idt[index].offset_l = offset & 0xFFFF;
	idt[index].offset_m = (offset >> 16) & 0xFFFF;
	idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
	idt[index].access = ACS_INT;
	idt[index].cero = 0;
	idt[index].other_cero = (uint64_t)0;
}
