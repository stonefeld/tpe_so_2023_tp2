global asm_lock
global asm_unlock

asm_lock:
  mov rax, 0
  mov al, 1
  xchg al,[rdi]
  cmp al, 0
  jne asm_lock
  ret

asm_unlock:
    mov byte [rdi],0
    ret
