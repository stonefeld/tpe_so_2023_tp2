global asm_getchar
global asm_putchar
global asm_datetime
global asm_clear
global asm_setcolor
global asm_printreg

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

asm_printreg:
    push rbp
    mov rbp,rsp

    mov rdi,3
    int 69h

    leave
    ret

asm_clear:
    push rbp
    mov rbp,rsp

    mov rdi,4
    int 69h

    leave
    ret

asm_datetime:
    push rbp
    mov rbp,rsp

    mov rdi, 5
    int 69h

    leave
    ret

asm_setcolor:
    push rbp
    mov rbp,rsp

    mov rdx,rsi
    mov rsi,rdi
    mov rdi,6
    int 69h

    leave
    ret
