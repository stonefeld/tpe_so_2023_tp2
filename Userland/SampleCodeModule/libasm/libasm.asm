global asm_testioe
global asm_testzde

asm_testioe:
    push rbp
    mov rbp,rsp
    mov cr6,rax
    leave
    ret

asm_testzde:
    push rbp
    mov rbp,rsp
    mov eax,10
    mov ebx,0
    div ebx
    leave
    ret
