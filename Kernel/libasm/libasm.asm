global asm_cpu_vendor
global asm_rtc_gettime
global asm_kbd_active
global asm_kbd_getkey
global asm_getsp
global asm_setreg
global asm_printreg
global asm_sound
global asm_nosound

extern exc_printreg

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

; devuelve el valor del stack-pointer
asm_getsp:
    push rbp
    mov rbp,rsp

    mov rax,rsp

    leave
    ret

asm_setreg:
    ; mov r15, 0
    ; mov r14, 1
    ; mov r13, 2
    ; mov r12, 3
    ; mov r11, 4
    ; mov r10, 5
    ; mov r9, 6
    ; mov r8, 7
    ; mov rsi, 8
    ; mov rdi, 9
    ; ;mov rbp, 10
    ; mov rdx, 11
    ; mov rcx, 12
    ; mov rbx, 13
    ; mov rax, 14
    mov [regs_stack],r15
    mov [regs_stack+1*8],r14
    mov [regs_stack+2*8],r13
    mov [regs_stack+3*8],r12
    mov [regs_stack+4*8],r11
    mov [regs_stack+5*8],r10
    mov [regs_stack+6*8],r9
    mov [regs_stack+7*8],r8
    mov [regs_stack+8*8],rsi
    mov [regs_stack+9*8],rdi
    mov [regs_stack+10*8],rbp
    mov [regs_stack+11*8],rdx
    mov [regs_stack+12*8],rcx
    mov [regs_stack+13*8],rbx
    mov [regs_stack+14*8],rax
    mov rax,[rsp]  ; rip
    mov [regs_stack+15*8],rax
    add rsp,8
    mov [regs_stack+16*8],rsp
    sub rsp,8
    mov [setreg_called],byte 1
    ret

asm_printreg:
    push rbp
    mov rbp,rsp

    mov rsi,rdi
    cmp byte [setreg_called],1
    jne .notcalled
    mov rdi,regs_stack
    jmp .next

.notcalled:
    mov rdi,0

.next:
    call exc_printreg

    leave
    ret

asm_sound:
    push rbp
    mov rbp,rsp

    mov al,182         ; Configuro altavoz
    out 43h,al

    mov ax,di          ; Frecuencia en bx
    out 42h,al         ; Envio byte - significativo
    mov al,ah
    out 42h,al         ; Envio byte + significativo

    in al,61h
    or al,3h
    out 61h,al

    leave
    ret

asm_nosound:
    push rbp
    mov rbp,rsp

    in al,61h
    and al,0fch
    out 61h,al

    leave
    ret

section .bss
regs_stack resb 17*8

section .data
setreg_called db 0
