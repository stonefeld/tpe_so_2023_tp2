#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef struct queue_adt* Queue;

Queue queue_create();
int queue_free(Queue queue);
int queue_add(Queue queue, int elem);
int queue_pop(Queue queue);
int queue_remove(Queue queue, int elem);
int queue_unblock(Queue queue);
int queue_unblock_all(Queue queue);

#endif
