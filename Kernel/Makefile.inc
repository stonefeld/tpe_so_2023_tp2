GCC=gcc
LD=ld
AR=ar
ASM=nasm

GCCFLAGS=-m64 -g -static -fno-exceptions -fno-asynchronous-unwind-tables -mno-mmx -mno-sse -mno-sse2 -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -mno-red-zone -Wall -ffreestanding -nostdlib -fno-common -std=c99 -fno-pic
ARFLAGS=rvs
ASMFLAGS=-felf64 -g
LDFLAGS=--warn-common -z max-page-size=0x1000

MEMORY_MANAGER=USE_LIST
