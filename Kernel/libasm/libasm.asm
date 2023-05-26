global asm_cpu_vendor
global asm_rtc_gettime
global asm_kbd_active
global asm_kbd_getkey
global asm_print_regs

extern tx_put_int
extern tx_put_word
extern tx_put_char


section .text

; devuelve el fabricante del cpu
asm_cpu_vendor:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 0
    cpuid

    mov [rdi], ebx
    mov [rdi + 4], edx
    mov [rdi + 8], ecx

    mov byte [rdi+13], 0

    mov rax, rdi

    pop rbx

    mov rsp, rbp
    pop rbp
    ret

; devuelve el tiempo en rtc
asm_rtc_gettime:
    push rbp
    mov rbp,rsp

    mov rax,rdi     ; argumento de tiempo (0:secs, 2:mins, 4:hs, ...)
    out 70h,al
    in al,71h

    leave
    ret

; devuelve si hay una tecla presionada o liberada
asm_kbd_active:
    push rbp
    mov rbp,rsp

    mov rax,0
    in al,64h
    and al,01h

    leave
    ret

; devuelve el código de una tecla en el teclado (si fue presionada o soltada)
asm_kbd_getkey:
    push rbp
    mov rbp,rsp

    mov rax,0
    in al,60h

    leave
    ret

asm_print_regs:
    push rbp  ; los registros están pusheados en el stack
    mov rbp,rsp

    mov r10, 0

    .loop:
        mov rdi, [REGS + r10]
        call tx_put_word

        add r10, 8

        mov rdi, [rbp + r10 + 8] ; el 8 es por el stackframe
        call tx_put_int

        mov rdi, 10
        call tx_put_char

        cmp r10, length

        jne .loop

    leave
    ret

section .data
    _RIP db " RIP = ", 0
    _RAX db " RAX = ", 0
    _RBX db " RBX = ", 0
    _RCX db " RCX = ", 0
    _RDX db " RDX = ", 0
    _RBP db " RBP = ", 0
    _RDI db " RDI = ", 0
    _RSI db " RSI = ", 0
    _RSP db " RSP = ", 0
    _R8 db  " R8 = ", 0
    _R9 db  " R9 = ", 0
    _R10 db " R10 = ", 0
    _R11 db " R11 = ", 0
    _R12 db " R12 = ", 0
    _R13 db " R13 = ", 0
    _R14 db " R14 = ", 0
    _R15 db " R15 = ", 0

    REGS dq _RIP, _RAX, _RBX, _RCX, _RDX, _RBP, _RDI, _RSI, _R8, _R9, _R10, _R11, _R12, _R13, _R14, _R15, 0
    length equ $-REGS
