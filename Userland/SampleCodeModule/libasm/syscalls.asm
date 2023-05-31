; i/o interaction
global asm_getchar
global asm_putchar
sys_read         equ 1
sys_write        equ 2

; drawing
global asm_draw
global asm_clear
global asm_setcolor
global asm_cursor
global asm_show_cursor
global asm_wallpaper
sys_draw         equ 3
sys_clear        equ 4
sys_color        equ 5
sys_cursor       equ 6
sys_show_cursor  equ 7
sys_wallpaper    equ 8

; properties
global asm_winwidth
global asm_winheight
global asm_fontwidth
global asm_fontheight
sys_winwidth     equ 9
sys_winheight    equ 10
sys_fontwidth    equ 11
sys_fontheight   equ 12

; system
global asm_ticked
global asm_printreg
global asm_datetime
sys_ticks        equ 13
sys_regs         equ 14
sys_rtc          equ 15

%macro syscall_handler 1
    push rbp
    mov rbp,rsp
    mov r9,r8
    mov r8,rcx
    mov rcx,rdx
    mov rdx,rsi
    mov rsi,rdi
    mov rdi,%1
    int 69h
    leave
    ret
%endmacro

asm_getchar:
    syscall_handler sys_read

asm_putchar:
    syscall_handler sys_write

asm_draw:
    syscall_handler sys_draw

asm_clear:
    syscall_handler sys_clear

asm_setcolor:
    syscall_handler sys_color

asm_cursor:
    syscall_handler sys_cursor

asm_show_cursor:
    syscall_handler sys_show_cursor

asm_wallpaper:
    syscall_handler sys_wallpaper

asm_winwidth:
    syscall_handler sys_winwidth

asm_winheight:
    syscall_handler sys_winheight

asm_fontwidth:
    syscall_handler sys_fontwidth

asm_fontheight:
    syscall_handler sys_fontheight

asm_ticked:
    syscall_handler sys_ticks

asm_printreg:
    syscall_handler sys_regs

asm_datetime:
    syscall_handler sys_rtc
