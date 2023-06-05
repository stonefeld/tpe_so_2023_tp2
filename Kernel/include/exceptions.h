#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

void exception_dispatcher(uint32_t exception, uint64_t* stack);
void exc_printreg(uint64_t* stack, uint32_t color, uint8_t exception);
void exc_set_restore_point(uint64_t ip, uint64_t sp);

#endif
