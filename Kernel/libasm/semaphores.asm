GLOBAL _lock
GLOBAL _unlock

_lock:
  mov rax, 0
  mov al, 1
  xchg al, [rdi]
  cmp al, 0
  jne _lock
  ret

_unlock:
    mov byte [rdi], 0
    ret