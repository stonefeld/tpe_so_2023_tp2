#include <memoryManager.h>
#include <queue.h>
#include <scheduler.h>

struct node
{
	uint8_t elem;
	struct node* next;
};

struct queue_adt
{
	struct node* first;
	struct node* last;
	int count;
};

static void free_rec(struct node* current);

Queue
queue_create()
{
	Queue queue = mm_alloc(sizeof(struct queue_adt));
	if (queue == NULL)
		return NULL;
	queue->first = NULL;
	queue->last = NULL;
	queue->count = 0;
	return queue;
}

int
queue_free(Queue queue)
{
	if (queue == NULL)
		return -1;
	if (queue->first != NULL)
		free_rec(queue->first);
	mm_free(queue);
	return 0;
}

int
queue_add(Queue queue, uint8_t elem)
{
	if (queue == NULL)
		return -1;

	struct node* node = mm_alloc(sizeof(struct node));
	if (node == NULL)
		return -1;
	node->elem = elem;
	node->next = NULL;

	struct node* current = queue->first;

	if (queue->first == NULL) {
		queue->first = node;
	} else {
		while (current->next != NULL)
			current = current->next;
		current->next = node;
	}

	queue->last = node;
	queue->count++;
	return 0;
}

int
queue_count(Queue queue)
{
	if (queue == NULL)
		return -1;
	return queue->count;
}

int
queue_contains(Queue queue, uint8_t elem)
{
	if (queue == NULL || queue->first == NULL)
		return -1;

	struct node* current = queue->first;
	while (current != NULL) {
		if (current->elem == elem)
			return 0;
		current = current->next;
	}
	return -1;
}

int
queue_unblock(Queue queue)
{
	if (queue == NULL)
		return -1;

	if (queue->first != NULL) {
		if (queue->first->next == NULL)
			queue->last = NULL;
		struct node* current = queue->first;
		queue->first = queue->first->next;
		sch_unblock(current->elem);
		mm_free(current);
		queue->count--;
	}
	return 0;
}

int
queue_unblock_all(Queue queue)
{
	if (queue == NULL)
		return -1;
	free_rec(queue->first);
	queue->first = NULL;
	queue->last = NULL;
	queue->count = 0;
	return 0;
}

static void
free_rec(struct node* node)
{
	if (node == NULL)
		return;
	free_rec(node->next);
	sch_unblock(node->elem);
	mm_free(node);
}
