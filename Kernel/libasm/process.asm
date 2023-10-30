global asm_create_process_context

asm_entry_point_wrapper:
    call rdx
    mov rax,42h
    int 80h

asm_create_process_context:
    push rbp
    mov rbp,rsp

    mov rsp,rdx  ; rsp
    push 0h      ; ss
    push rdx     ; rsp
    push 202h    ; rflags
    push 8h      ; cs
    push asm_entry_point_wrapper

    push 1h      ; rbx
    push 2h      ; rcx
    push rcx     ; rdx (entry_point)
    push rdx     ; rbp (rbp = rsp inicialmente)
    push rdi     ; argc
    push rsi     ; argv
    push 8h      ; r8
    push 9h      ; r9
    push 0Ah     ; r10
    push 0Bh     ; r11
    push 0Ch     ; r12
    push 0Dh     ; r13
    push 0Eh     ; r14
    push 0Fh     ; r15
    push 10h     ; rax

    mov rax,rsp
    mov rsp,rbp
    pop rbp
    ret
