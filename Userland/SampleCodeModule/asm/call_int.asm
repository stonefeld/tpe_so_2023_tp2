GLOBAL syscall


syscall:
    push rbp
    mov rbp,rsp
    
    int 69h

    mov rsp, rbp
    pop rbp
    ret 