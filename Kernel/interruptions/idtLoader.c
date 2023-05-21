#include <idtLoader.h>


/*
    En Bootloader/Pure64/src/pure64.asm encontramos:

    IDTR64:					; Interrupt Descriptor Table Register
        dw 256*16-1			; limit of IDT (size minus one) (4096 bytes - 1)
        dq 0x0000000000000000		; linear address of IDT
    ; -----------------
*/

// extracted from https://wiki.osdev.org/IDT



typedef struct InterruptDescriptor64 {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
   uint32_t offset_3;        // offset bits 32..63
   uint32_t zero;            // reserved
} idtEntry;

idtEntry * idt = (idtEntry *) 0;        // en la direccion lineal 0 se va a encontrar la IDT de 255 entradas. 


static void idt_entry_init(int index, uint64_t func_address);  // func_address is a pointer to the Interruption routine

// here we load the idt index and the pointer to the function we want to call when 'int index' is executed
void load_idt(){
    
    _cli(); // disable interrupts

    //idt_entry_init(0x00, 1);
    idt_entry_init(0x69, (uint64_t) &syscallHandler);  // en vez de 1 -> la dirección al syscall handler.
                              // ahora int 69h ocuparía el rol de int 80h

    //...
    _sti(); // enable interrupts
}


static void idt_entry_init (int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_1 = offset & 0xFFFF;
  idt[index].offset_2 = (offset >> 16) & 0xFFFF;
  idt[index].offset_3 = (offset >> 32) & 0xFFFFFFFF;
  //idt[index].type_attributes = ;
  idt[index].zero = 0;
  idt[index].ist = (uint64_t) 0;
}