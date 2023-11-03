#include <libc.h>
#include <memoryManager.h>
#include <queue.h>
#include <scheduler.h>
#include <semaphore.h>
#include <stdint.h>

typedef struct
{
	uint8_t value;
	int8_t lock;
	uint8_t linked_processes;
	const char* name;
	Queue waiting_processes;
} SemInfo;

static SemInfo* semaphores[MAX_SEMAPHORES] = {};

extern int _lock(int8_t* lock);
extern void _unlock(int8_t* lock);

static int search_next();
static int search_semaphore(const char* name);
static int create_semaphore(int idx, int initial_value);
static void sem_free(semaphore sem);
static int is_valid_id(semaphore sem);
static int is_valid_sem(semaphore sem);

int8_t big_lock;

int
sem_init(semaphore sem, uint8_t initial_value)
{
	big_lock = 0;

	if (!is_valid_id(sem) || create_semaphore(sem, initial_value) == -1) {
		return -1;
	}
	return 0;
}

semaphore
sem_open(const char* name, uint8_t initial_value)
{
	_lock(&big_lock);
	int i = search_semaphore(name);
	if (i != -1) {
		semaphores[i]->linked_processes++;
		_unlock(&big_lock);
		return (semaphore)i;
	}

	i = search_next();
	if (i == -1) {
		_unlock(&big_lock);
		return -1;
	}

	int idx = create_semaphore(i, initial_value);
	if (idx == -1) {
		_unlock(&big_lock);
		return -1;
	}
	semaphores[idx]->name = name;
	_unlock(&big_lock);
	return (semaphore)idx;
}

int
sem_close(semaphore sem)
{
	_lock(&big_lock);
	if (!is_valid_sem(sem)) {
		_unlock(&big_lock);
		return -1;
	}
	_lock(&(semaphores[sem]->lock));
	_unlock(&big_lock);
	if (semaphores[sem]->linked_processes == 1) {
		sem_free(sem);
		return 0;
	}

	semaphores[sem]->linked_processes -= 1;
	_unlock(&semaphores[sem]->lock);
	return 0;
}

int
sem_wait(semaphore sem)
{
	_lock(&big_lock);
	if (!is_valid_sem(sem)) {
		_unlock(&big_lock);
		return -1;
	}
	_lock(&semaphores[sem]->lock);
	_unlock(&big_lock);

	uint8_t pid = sch_get_current_pid();
	while (semaphores[sem]->value == 0) {
		queue_add(semaphores[sem]->waiting_processes, pid);
		_unlock(&semaphores[sem]->lock);
		sch_block(pid);
		sch_yield();
		// Se queda esperando a que se desloquee
		_lock(&semaphores[sem]->lock);
	}

	semaphores[sem]->value--;
	_unlock(&semaphores[sem]->lock);
	return 0;
}

int
sem_post(semaphore sem)
{
	_lock(&big_lock);
	if (!is_valid_sem(sem)) {
		_unlock(&big_lock);
		return -1;
	}
	_lock(&semaphores[sem]->lock);
	_unlock(&big_lock);

	semaphores[sem]->value++;
	queue_remove(semaphores[sem]->waiting_processes);
	_unlock(&semaphores[sem]->lock);
	return 0;
}

static int
search_semaphore(const char* name)
{
	for (int i = 0; i < MAX_SEMAPHORES; i++) {
		if (strcmp(name, semaphores[i]->name) == 0) {
			return i;
		}
	}
	return -1;
}

static int
search_next()
{
	for (int i = 0; i < MAX_SEMAPHORES; i++) {
		if (semaphores[i] == NULL) {
			return i;
		}
	}
	return -1;
}

static int
create_semaphore(int idx, int initial_value)
{
	semaphores[idx] = mm_alloc(sizeof(SemInfo));
	if (semaphores[idx] == NULL) {
		return -1;
	}

	semaphores[idx]->value = initial_value;
	semaphores[idx]->linked_processes = 1;
	semaphores[idx]->waiting_processes = queue_create();

	if (semaphores[idx]->waiting_processes == NULL) {
		mm_free(semaphores[idx]);
		semaphores[idx] = NULL;
		return -1;
	}

	return (semaphore)idx;
}

static void
sem_free(semaphore sem)
{
	queue_free(semaphores[sem]->waiting_processes);
	mm_free(semaphores[sem]);
	semaphores[sem] = NULL;
}

static int
is_valid_id(semaphore sem)
{
	return sem > 0 && sem < MAX_SEMAPHORES;
}

static int
is_valid_sem(semaphore sem)
{
	return is_valid_id(sem) && semaphores[sem] != NULL;
}
