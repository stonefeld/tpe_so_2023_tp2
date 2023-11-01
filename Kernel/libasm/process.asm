global asm_create_process_context

asm_entry_point_wrapper:
    call rdx
    mov rdi,10h
    int 80h

asm_create_process_context:
    push rbp
    mov rbp,rsp

    mov rsp,rdx
    push 0h     ; SS
    push rdx    ; RSP
    push 202h   ; RFLAGS
    push 8h     ; CS
    push asm_entry_point_wrapper

    push 01h    ; rax
    push 02h    ; rbx
    push 03h    ; rcx
    push rcx    ; rdx (entry_point)
    push rdx    ; rbp (rbp = rsp inicialmente)
    push rdi    ; rdi --> argc
    push rsi    ; rsi --> argv
    push 08h    ; r8
    push 09h    ; r9
    push 0Ah    ; r10
    push 0Bh    ; r11
    push 0Ch    ; r12
    push 0Dh    ; r13
    push 0Eh    ; r14
    push 0Fh    ; r15

    mov rax,rsp

    mov rsp,rbp
    pop rbp
    ret
