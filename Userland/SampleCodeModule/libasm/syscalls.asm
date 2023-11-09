; i/o interaction
global asm_exit
global asm_read
global asm_write
global asm_close
; system
global asm_time
global asm_sleep

; processes
global asm_execve
global asm_waitpid
global asm_getpid
global asm_ps
global asm_nice
global asm_kill
global asm_block
global asm_yield

; pipes
global asm_pipe
global asm_pipe_open
global asm_pipe_unlink
global asm_pipe_status

; semaphores
global asm_sem_open
global asm_sem_wait
global asm_sem_post
global asm_sem_close

; memory
global asm_meminfo
global asm_malloc
global asm_free
global asm_realloc

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

asm_exit: syscall_handler 1
asm_read: syscall_handler 3
asm_write: syscall_handler 4
asm_close: syscall_handler 6

asm_time: syscall_handler 13
asm_sleep: syscall_handler 14

asm_execve: syscall_handler 2
asm_waitpid: syscall_handler 7
asm_getpid: syscall_handler 20
asm_ps: syscall_handler 21
asm_nice: syscall_handler 34
asm_kill: syscall_handler 37
asm_block: syscall_handler 38
asm_yield: syscall_handler 158

asm_pipe: syscall_handler 42
asm_pipe_open: syscall_handler 43
asm_pipe_unlink: syscall_handler 44
asm_pipe_status: syscall_handler 45

asm_sem_open: syscall_handler 50
asm_sem_wait: syscall_handler 51
asm_sem_post: syscall_handler 52
asm_sem_close: syscall_handler 53

asm_meminfo: syscall_handler 89
asm_malloc: syscall_handler 90
asm_free: syscall_handler 91
asm_realloc: syscall_handler 92