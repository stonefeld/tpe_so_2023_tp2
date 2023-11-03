; i/o interaction
global asm_read
global asm_write
sys_read         equ 3
sys_write        equ 4

; drawing
global asm_show_cursor
sys_show_cursor  equ 6

; system
global asm_time
sys_time         equ 11

; memory
global asm_malloc
global asm_free
global asm_realloc
sys_malloc       equ 90
sys_free         equ 91
sys_realloc      equ 92

%macro syscall_handler 1
    push rbp
    mov rbp,rsp
    mov r9,r8
    mov r8,rcx
    mov rcx,rdx
    mov rdx,rsi
    mov rsi,rdi
    mov rdi,%1
    int 80h
    leave
    ret
%endmacro

asm_read:
    syscall_handler sys_read

asm_write:
    syscall_handler sys_write

asm_show_cursor:
    syscall_handler sys_show_cursor

asm_time:
    syscall_handler sys_time

asm_malloc:
    syscall_handler sys_malloc

asm_free:
    syscall_handler sys_free

asm_realloc:
    syscall_handler sys_realloc
