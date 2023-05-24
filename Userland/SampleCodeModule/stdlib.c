#include <stdlib.h>
#include <stdint.h>

extern void syscall(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

int getChar(char *c){
    syscall(4, c,0,0,0,0);  // algo así, voy a llamar a read -> rdi = SYS_READ

}
void printf(char* str){

}
void putChar(char c){

}