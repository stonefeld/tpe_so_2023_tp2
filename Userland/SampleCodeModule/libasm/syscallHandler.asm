global asm_getchar
global asm_putchar
<<<<<<< Updated upstream
global asm_time
=======
global asm_clear
>>>>>>> Stashed changes

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
asm_clear:
    push rbp
    mov rbp,rsp

    mov rdi,4
    int 69h

    leave 
    rets
