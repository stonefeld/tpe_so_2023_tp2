#include <philosophers.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <test_syscalls.h>
#include <test_util.h>

#define FORK_SEM_NAME     "sem_fork"
#define MAX_PHILOSOPHERS  12
#define MIN_PHILOSOPHERS  5
#define EATING_TIME_MIN   2000
#define EATING_TIME_MAX   5000
#define THINKING_TIME_MIN 2000
#define THINKING_TIME_MAX 5000

typedef enum
{
	EATING = 0,
	HUNGRY,
	THINKING,
} PhilosopherState;

typedef struct
{
	char* philo_name;
	PhilosopherState philo_state;
	int philo_pid;
} Philosopher;

extern Color color;

static char* philo_names[MAX_PHILOSOPHERS] = {
	"Tales de Mileto", "Heráclito", "Anaxímenes",  "Pitágoras", "Parménides", "Demócrito",
	"Sócrates",        "Platón",    "Aristóteles", "Epicuro",   "Descartes",  "Marx Verstappen",
};

static Philosopher philos[MAX_PHILOSOPHERS];
static int philo_count;

static int forks[MAX_PHILOSOPHERS];
static int sem_big_fork;

static int get_thinking_time();
static int get_eating_time();

static int philosopher(int argc, char** argv);

static void philo_think(int idx);
static void philo_eat(int idx);

static int add_philo(int idx);
static int add_fork(int idx);

static void terminate_philos();
static void terminate_forks();

static void print_state();

static char buff[2];

int
init_philo_dilemma(int argc, char* argv[])
{
	if (argc != 1)
		return -1;

	philo_count = str_to_int(argv[0]);

	if (philo_count < MIN_PHILOSOPHERS || philo_count > MAX_PHILOSOPHERS) {
		puts("Philosopheres number should be between 5 and 12\n", color.error);
		return -1;
	}

	for (int i = 0; i < philo_count; i++) {
		philos[i].philo_name = NULL;
		forks[i] = -1;
	}

	for (int i = 0; i < philo_count; i++) {
		if (add_philo(i) == -1)
			return -1;
		if (add_fork(i) == -1)
			return -1;
	}

	sem_big_fork = asm_sem_open("sem_big_fork", 1);

	if (sem_big_fork == -1) {
		puts("Could not initialized semaphore\n", color.error);
		philo_kill();
		return -1;
	}

	for (int i = 0; i < philo_count; i++)
		asm_waitpid(philos[i].philo_pid);

	return 0;
}

void
philo_kill()
{
	terminate_philos();
	terminate_forks();
}

static int
philosopher(int argc, char** argv)
{
	int philo_idx = str_to_int(argv[0]);
	philo_think(philo_idx);
	philo_eat(philo_idx);
	philos[philo_idx].philo_state = THINKING;
	return 0;
}

static void
philo_think(int idx)
{
	philos[idx].philo_state = THINKING;
	print_state();
	asm_sleep(get_thinking_time());
}

static void
philo_eat(int idx)
{
	philos[idx].philo_state = HUNGRY;
	print_state();

	int neighbour_idx = (idx + 1) % philo_count;
	asm_sem_wait(sem_big_fork);
	asm_sem_wait(forks[idx]);
	asm_sem_wait(forks[neighbour_idx]);
	asm_sem_post(sem_big_fork);

	philos[idx].philo_state = EATING;
	print_state();
	asm_sleep(get_eating_time());

	asm_sem_post(forks[neighbour_idx]);
	asm_sem_post(forks[idx]);
}

static int
add_philo(int idx)
{
	if (philos[idx].philo_name != NULL) {
		puts("Failed adding philosopher\n", color.error);
		return -1;
	}

	philos[idx].philo_name = philo_names[idx];
	philos[idx].philo_state = THINKING;
	int_to_str(idx, buff);
	char* argv[] = { buff };

	ProcessCreateInfo create_info = {
		.argc = 1,
		.argv = argv,
		.name = philo_names[idx],
		.entry_point = philosopher,
		.is_fg = 0,
		.priority = 0,
	};
	philos[idx].philo_pid = asm_execve(&create_info);

	return 0;
}

static int
add_fork(int idx)
{
	if (forks[idx] != -1)
		return -1;

	int_to_str(idx, buff);
	char* sem_name = strcat(FORK_SEM_NAME, buff);

	forks[idx] = asm_sem_open(sem_name, 1);
	asm_free(sem_name);
	if (forks[idx] == -1) {
		puts("Could not initialize semaphore\n", color.error);
		philo_kill();
		return -1;
	}

	return 0;
}

static void
terminate_philos()
{
	for (int i = 0; i < philo_count; i++) {
		asm_kill(philos[i].philo_pid);
		philos[i].philo_name = NULL;
	}
}

static void
terminate_forks()
{
	for (int i = 0; i < MAX_PHILOSOPHERS && forks[i] >= -1; i++)
		asm_sem_close(forks[i]);

	asm_sem_close(sem_big_fork);

	for (int i = 0; i < philo_count; i++)
		forks[i] = -1;
}

static void
print_state()
{
	for (int i = 0; i < philo_count; ++i) {
		putchar(philos[i].philo_state == HUNGRY ? 'h' : philos[i].philo_state == EATING ? 'E' : '-', color.output);
		putchar(' ', color.output);
	}
	puts("\n", color.output);
}

static int
get_thinking_time()
{
	return THINKING_TIME_MIN + GetUniform(THINKING_TIME_MAX) % (THINKING_TIME_MAX - THINKING_TIME_MIN + 1);
}

static int
get_eating_time()
{
	return EATING_TIME_MIN + GetUniform(EATING_TIME_MAX) % (EATING_TIME_MAX - EATING_TIME_MIN + 1);
}
