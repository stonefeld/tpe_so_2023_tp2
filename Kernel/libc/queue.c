#include <memoryManager.h>
#include <queue.h>
#include <scheduler.h>

struct node
{
	int elem;
	struct node* next;
};

struct queue_adt
{
	struct node* first;
	struct node* last;
	int count;
};

static void free_rec(struct node* current);
static struct node* remove_rec(struct node* current, int elem);

Queue
queue_create()
{
	Queue queue = mm_alloc(sizeof(struct queue_adt));
	if (queue == NULL) {
		mm_free(queue);
		return NULL;
	}
	queue->first = NULL;
	queue->last = NULL;
	queue->count = 0;
	return queue;
}

int
queue_free(Queue queue)
{
	int ret = queue_unblock_all(queue);
	mm_free(queue);
	return ret;
}

int
queue_add(Queue queue, int elem)
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
queue_pop(Queue queue)
{
	if (queue == NULL)
		return -1;

	if (queue->first != NULL) {
		if (queue->first->next == NULL)
			queue->last = NULL;
		struct node* current = queue->first;
		int pid = current->elem;
		queue->first = queue->first->next;
		mm_free(current);
		queue->count--;
		return pid;
	}
	return -1;
}

int
queue_remove(Queue queue, int elem)
{
	if (queue == NULL)
		return -1;
	queue->first = remove_rec(queue->first, elem);
	return 0;
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

static struct node*
remove_rec(struct node* current, int elem)
{
	if (current == NULL)
		return NULL;
	if (current->elem == elem) {
		struct node* ret = current->next;
		mm_free(current);
		return ret;
	}
	current->next = remove_rec(current->next, elem);
	return current;
}
