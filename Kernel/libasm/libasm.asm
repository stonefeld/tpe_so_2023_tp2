global _cpuVendor
global _rtcGettime
global _keyboardActive
global _keyboardGetkey

section .text

; devuelve el fabricante del cpu
_cpuVendor:
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
_rtcGettime:
    push rbp
    mov rbp,rsp

    mov rax,rdi     ; argumento de tiempo (0:secs, 2:mins, 4:hs, ...)
    out 70h,al
    in al,71h

    leave
    ret

; devuelve si hay una tecla presionada o liberada
_keyboardActive:
    push rbp
    mov rbp,rsp

    mov rax,0
    in al,64h
    and al,01h

    leave
    ret

; devuelve el código de una tecla en el teclado (si fue presionada o soltada)
_keyboardGetkey:
    push rbp
    mov rbp,rsp

    mov rax,0
    in al,60h

    leave
    ret
