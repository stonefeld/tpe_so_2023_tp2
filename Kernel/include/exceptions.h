#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

/*
 * Wrapper de las funciones que manejan las excepciones.
 */
void exception_dispatcher(uint32_t exception, uint64_t* stack);

#endif
