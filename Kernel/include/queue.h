#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef struct queue_adt* Queue;

Queue queue_new();
int queue_free(Queue queue);
int queue_add(Queue queue, uint8_t elem);
int queue_count(Queue queue);
int queue_remove(Queue queue);
int queue_remove_all(Queue queue);
int queue_contains(Queue queue, uint8_t elem);

#endif