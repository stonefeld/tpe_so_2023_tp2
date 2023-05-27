global asm_getchar
global asm_putchar
global asm_datetime
global asm_clear
global asm_setcolor
global asm_printreg
global asm_draw
global asm_winwidth
global asm_winheight
global asm_getticks
global asm_cursor
global asm_kreleased

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

asm_draw
    push rbp
    mov rbp,rsp

    mov r8,rcx
    mov rcx,rdx
    mov rdx,rsi
    mov rsi,rdi

    mov rdi,7
    int 69h

    leave
    ret

asm_winwidth:
    push rbp
    mov rbp,rsp

    mov rdi,8
    int 69h

    leave
    ret

asm_winheight:
    push rbp
    mov rbp,rsp

    mov rdi,9
    int 69h

    leave
    ret

asm_getticks:
    push rbp
    mov rbp,rsp

    mov rdi,10
    int 69h

    leave
    ret

asm_cursor:
    push rbp
    mov rbp,rsp

    mov rdx,rsi
    mov rsi,rdi
    mov rdi,11
    int 69h

    leave
    ret

asm_kreleased:
    push rbp
    mov rbp,rsp

    mov rdi,12
    int 69h

    leave
    ret
