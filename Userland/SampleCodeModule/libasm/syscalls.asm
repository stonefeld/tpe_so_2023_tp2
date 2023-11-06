; i/o interaction
global asm_exit
global asm_read
global asm_write
sys_exit         equ 1
sys_read         equ 3
sys_write        equ 4

; system
global asm_time
sys_time         equ 13

; processes
global asm_execve
global asm_getpid
global asm_ps
global asm_nice
global asm_kill
sys_execve       equ 2
sys_getpid       equ 20
sys_ps           equ 21
sys_nice         equ 34
sys_kill         equ 37

; pipes
global asm_pipe
global asm_pipe_open
global asm_pipe_unlink
global asm_pipe_status
sys_pipe         equ 42
sys_pipe_open    equ 43
sys_pipe_unlink  equ 44
sys_pipe_status  equ 45

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

asm_time:
    syscall_handler sys_time

asm_execve:
    syscall_handler sys_execve

asm_getpid:
    syscall_handler sys_getpid

asm_ps:
    syscall_handler sys_ps

asm_nice:
    syscall_handler sys_nice

asm_kill:
    syscall_handler sys_kill

asm_pipe:
    syscall_handler sys_pipe

asm_pipe_open:
    syscall_handler sys_pipe_open

asm_pipe_unlink:
    syscall_handler sys_pipe_unlink

asm_pipe_status:
    syscall_handler sys_pipe_status

asm_malloc:
    syscall_handler sys_malloc

asm_free:
    syscall_handler sys_free

asm_realloc:
    syscall_handler sys_realloc
