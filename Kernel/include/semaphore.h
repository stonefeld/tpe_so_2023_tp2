#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>

#define MAX_SEMAPHORES 32

typedef int Semaphore;

Semaphore sem_open(const char* name, uint8_t init_value);
int sem_close(Semaphore sem);

int sem_wait(Semaphore sem);
int sem_post(Semaphore sem);

#endif
