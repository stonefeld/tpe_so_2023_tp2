global asm_getchar
global asm_putchar
global asm_time

asm_getchar:
    push rbp
    mov rbp,rsp

    mov rdi,1
    int 69h

    leave
    ret

asm_putchar:
    push rbp
    mov rbp,rsp

    mov rsi,rdi
    mov rdi,2
    int 69h

    leave
    ret

asm_time:
    push rbp
    mov rbp,rsp

    mov rdi, 5
    int 69h

    leave
    ret