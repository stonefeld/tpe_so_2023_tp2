global asm_getchar
global asm_putchar

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
