#include <idtLoader.h>
#include <lib.h>

/*
    En Bootloader/Pure64/src/pure64.asm encontramos:

    IDTR64:					; Interrupt Descriptor Table Register
        dw 256*16-1			; limit of IDT (size minus one) (4096 bytes - 1)
        dq 0x0000000000000000		; linear address of IDT
    ; -----------------
*/
// extracted from https://wiki.osdev.org/IDT

struct interrupt_descriptor_64
{
	uint16_t offset_1;        // offset bits 0..15
	uint16_t selector;        // a code segment selector in GDT or LDT
	uint8_t ist;              // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
	uint8_t type_attributes;  // gate type, dpl, and p fields
	uint16_t offset_2;        // offset bits 16..31
	uint32_t offset_3;        // offset bits 32..63
	uint32_t zero;            // reserved
};

typedef struct interrupt_descriptor_64 IDTEntry;
IDTEntry* idt = (IDTEntry*)0;  // en la direccion lineal 0 se va a encontrar la IDT de 255 entradas.

// func_address es el puntero a la rutina de interrupción.
static void idt_entry_init(int index, uint64_t func_address);

// here we load the idt index and the pointer to the function we want to call
// when 'int index' is executed
void
load_idt()
{
	// deshabilitamos las interrupciones
	interrupt_dis();

	// idt_entry_init(0x00, 1);
	// registramos 69h para el handler de los syscalls
	idt_entry_init(0x69, (uint64_t)&syscall_handler);

	// rehabilitamos las interrupciones
	interrupt_en();
}

static void
idt_entry_init(int index, uint64_t offset)
{
	idt[index].selector = 0x08;
	idt[index].offset_1 = offset & 0xFFFF;
	idt[index].offset_2 = (offset >> 16) & 0xFFFF;
	idt[index].offset_3 = (offset >> 32) & 0xFFFFFFFF;
	// idt[index].type_attributes = ;
	idt[index].zero = 0;
	idt[index].ist = (uint64_t)0;
}
