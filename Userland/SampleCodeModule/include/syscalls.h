/*
 * Estas funciones están definidas en libasm/libasm.asm y utilizan
 * syscalls para sus respectivas llamadas.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

extern uint32_t asm_read(int fd, char* buf, uint32_t size);
extern uint32_t asm_write(int fd, char* buf, uint32_t size, uint32_t color);

/*
 * Muestra u oculta el cursor.
 */
extern void asm_show_cursor(uint8_t show);

/*
 * Imprime fecha y hora actual.
 */
extern void asm_time(uint32_t color);

extern void* asm_malloc(size_t size);
extern void asm_free(void* ptr);
extern void* asm_realloc(void* ptr, size_t size);

#endif
