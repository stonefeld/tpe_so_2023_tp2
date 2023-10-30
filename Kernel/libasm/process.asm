global asm_create_process_context

asm_entry_point_wrapper:
    call rdx
    mov rax, 42h
    int 80h

asm_create_process_context:
    push rbp
    mov rbp, rsp

    mov rsp, rdx ; rsp
    push 0h     ; SS
    push rdx     ; RSP
    push 202h   ; RFLAGS
    push 8h     ; CS
    push asm_entry_point_wrapper     ; entryPoint (wrapped)

    push 01h    ; RBX
    push 02h    ; RCX
    push rcx     ; RDX (entryPoint)
    push rdx     ; RBP (initally, RBP = RSP)
	push rdi     ; RDI --> argc
	push rsi     ; RSI --> argv
	push 08h    ; R8
	push 09h    ; r9
	push 0Ah    ; r10
	push 0Bh    ; r11
	push 0Ch    ; r12
	push 0Dh    ; r13
	push 0Eh    ; r14
	push 0Fh    ; r15
	push 10h    ; rax

    mov rax, rsp ; return the currentRSP of the process

    mov rsp, rbp
    pop rbp
    ret
