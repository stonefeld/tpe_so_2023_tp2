#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>

#define MAX_SEMAPHORES 255

typedef uint8_t semaphore;

int sem_init(semaphore sem, uint8_t initialValue);

semaphore sem_open(const char* name, uint8_t initialValue);
int sem_close(semaphore sem);

int sem_wait(semaphore sem);
int sem_post(semaphore sem);

#endif