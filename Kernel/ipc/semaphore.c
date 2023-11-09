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

static SemInfo* semaphores[MAX_SEMAPHORES];
static int8_t big_lock;

static int search_next();
static int search_semaphore(const char* name);
static int create(int idx, int initial_value);
static void free(Semaphore sem);
static int is_valid_id(Semaphore sem);
static int is_valid_sem(Semaphore sem);

extern int asm_lock(int8_t* lock);
extern void asm_unlock(int8_t* lock);

Semaphore
sem_open(const char* name, uint8_t initial_value)
{
	asm_lock(&big_lock);
	int i = search_semaphore(name);
	if (i != -1) {
		semaphores[i]->linked_processes++;
		asm_unlock(&big_lock);
		return (Semaphore)i;
	}

	i = search_next();
	if (i == -1) {
		asm_unlock(&big_lock);
		return -1;
	}

	int idx = create(i, initial_value);
	if (idx == -1) {
		asm_unlock(&big_lock);
		return -1;
	}
	semaphores[idx]->name = name;
	asm_unlock(&big_lock);
	return (Semaphore)idx;
}

int
sem_close(Semaphore sem)
{
	asm_lock(&big_lock);
	if (!is_valid_sem(sem)) {
		asm_unlock(&big_lock);
		return -1;
	}
	asm_lock(&(semaphores[sem]->lock));
	asm_unlock(&big_lock);
	if (semaphores[sem]->linked_processes == 1) {
		free(sem);
		return 0;
	}

	semaphores[sem]->linked_processes -= 1;
	asm_unlock(&semaphores[sem]->lock);
	return 0;
}

int
sem_wait(Semaphore sem)
{
	asm_lock(&big_lock);
	if (!is_valid_sem(sem)) {
		asm_unlock(&big_lock);
		return -1;
	}
	asm_lock(&semaphores[sem]->lock);
	asm_unlock(&big_lock);

	uint8_t pid = sch_get_current_pid();
	while (semaphores[sem]->value == 0) {
		queue_add(semaphores[sem]->waiting_processes, pid);
		asm_unlock(&semaphores[sem]->lock);
		sch_block(pid);
		sch_yield();
		// Se queda esperando a que se desloquee
		asm_lock(&semaphores[sem]->lock);
	}

	semaphores[sem]->value--;
	asm_unlock(&semaphores[sem]->lock);
	return 0;
}

int
sem_post(Semaphore sem)
{
	asm_lock(&big_lock);
	if (!is_valid_sem(sem)) {
		asm_unlock(&big_lock);
		return -1;
	}
	asm_lock(&semaphores[sem]->lock);
	asm_unlock(&big_lock);

	semaphores[sem]->value++;
	queue_unblock(semaphores[sem]->waiting_processes);
	asm_unlock(&semaphores[sem]->lock);
	return 0;
}

static int
search_semaphore(const char* name)
{
	for (int i = 0; i < MAX_SEMAPHORES; i++) {
		if (semaphores[i] != NULL && strcmp(name, semaphores[i]->name) == 0)
			return i;
	}
	return -1;
}

static int
search_next()
{
	for (int i = 0; i < MAX_SEMAPHORES; i++) {
		if (semaphores[i] == NULL)
			return i;
	}
	return -1;
}

static int
create(int idx, int initial_value)
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

	return (Semaphore)idx;
}

static void
free(Semaphore sem)
{
	queue_free(semaphores[sem]->waiting_processes);
	mm_free(semaphores[sem]);
	semaphores[sem] = NULL;
}

static int
is_valid_id(Semaphore sem)
{
	return sem >= 0 && sem < MAX_SEMAPHORES;
}

static int
is_valid_sem(Semaphore sem)
{
	return is_valid_id(sem) && semaphores[sem] != NULL;
}
