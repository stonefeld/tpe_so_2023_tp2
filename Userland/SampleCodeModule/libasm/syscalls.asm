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

;pipes
global asm_pipe
global asm_pipe_unlink
global asm_pipe_open
global asm_pipe_status
sys_pipe        equ 17
sys_pipe_unlink  equ 18
sys_pipe_open   equ 19
sys_pipe_status equ 20

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

asm_pipe:
    syscall_handler sys_pipe
asm_pipe_unlink:
    syscall_handler sys_pipe_unlink
asm_pipe_open:
    syscall_handler sys_pipe_open
asm_pipe_status:
    syscall_handler sys_pipe_status
