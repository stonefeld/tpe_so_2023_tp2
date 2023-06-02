global asm_testioe
global asm_testzde

asm_testioe:
    push rbp
    mov rbp,rsp
    
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
    ; mov rbp, 10
    ; mov rdx, 11
    ; mov rcx, 12
    ; mov rbx, 13
    ; mov rax, 14

    ; stack ptr = [dir_ret, 0 ,r15,r14,r13,r12,r11,r10,r9,r8,rsi,rdi,rbp,rdx,rcx,rbx,rax,rbp, RIP, CS, EFLAGS, RSP(viejo)]
    mov cr6,rax
    leave
    ret

asm_testzde:
    push rbp
    mov rbp,rsp
    
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
    ; mov rbp, 10
    ; mov rdx, 11
    ; mov rcx, 12
    ; mov rbx, 13
    ; mov rax, 14
    
    mov eax,10
    div ebx
    leave
    ret
