global cpu_vendor
global gettime
global getkey
global interrupt_en
global interrupt_dis

section .text

cpu_vendor:
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

gettime:
    push rbp
    mov rbp,rsp

    mov rax,rdi     ; argumento de tiempo (0:secs, 2:mins, 4:hs, ...)
    out 70h,al
    in al,71h

    leave
    ret

getkey:
    push rbp
    mov rbp,rsp

    mov rax,0

.loop:
    in al,64h
    mov cl,al
    and al,1h
    cmp al,0
    je .loop
    in al,60h

    leave
    ret

interrupt_dis:
    cli
    ret

interrupt_en:
    sti
    ret
