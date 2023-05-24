global _cli
global _sti
global _hlt

global _picMasterMask
global _picSlaveMask
global _haltcpu

global _irq00Handler
global _irq01Handler
global _irq02Handler
global _irq03Handler
global _irq04Handler
global _irq05Handler

global _sysCallHandler
global _exception0Handler

extern irq_dispatcher
extern exception_dispatcher
extern syscall_dispatcher

section .text

%macro pushState 0
   push rbx
   push rcx
   push rdx
   push rbp
   push rdi
   push rsi
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15
   push rax
%endmacro

%macro popState 0
   pop rax
   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rsi
   pop rdi
   pop rbp
   pop rdx
   pop rcx
   pop rbx
%endmacro

%macro irqHandlerMaster 1
   push rsp
   pushState

   mov rdi,%1 ; pasaje de parametro
   call irq_dispatcher

   ; signal pic EOI (End of Interrupt)
   mov al,20h
   out 20h,al

   popState
   pop rsp
   iretq
%endmacro

%macro exceptionHandler 1
   pushState

   mov rdi,%1 ; pasaje de parametro
   call exception_dispatcher

   popState
   iretq
%endmacro


_hlt:
    sti
    hlt
    ret

_cli:
    cli
    ret

_sti:
    sti
    ret

_picMasterMask:
    push rbp
    mov rbp,rsp

    mov ax,di
    out 21h,al

    pop rbp
    retn

_picSlaveMask:
    push rbp
    mov rbp,rsp

    mov ax,di  ; ax = mascara de 16 bits
    out 0A1h,al

    pop rbp
    retn

; 8254 Timer (Timer Tick)
_irq00Handler:
    irqHandlerMaster 0

; Keyboard interrupt. IRQ 0x01, INT 0x21
_irq01Handler:
    irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
   irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
   irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
   irqHandlerMaster 4

; USB
_irq05Handler:
   irqHandlerMaster 5

; Zero Division Exception
_exception0Handler:
   exceptionHandler 0

_sysCallHandler:
   pushState
   pop rax
   call syscall_dispatcher
   push rax
   popState
   iretq

_haltcpu:
   cli
   hlt
   ret
