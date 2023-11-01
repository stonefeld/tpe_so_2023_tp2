#include <libc.h>
#include <memoryManager.h>
#include <process.h>
#include <scheduler.h>

typedef struct
{
	void *stack_end, *stack_start;
	uint8_t is_fg;
	char* name;

	void** mem;
	uint32_t mem_count, mem_bufsize;

	int argc;
	char** argv;
} ProcessContext;

static ProcessContext processes[MAX_PROCESSES];

static uint8_t valid_name(const char* name);
static uint8_t get_process_from_pid(int pid, ProcessContext** process);

static uint8_t
valid_name(const char* name)
{
	if (name == NULL)
		return 0;

	char c;
	for (int i = 0; i < MAX_NAME_LEN; i++) {
		c = name[i];
		if (c == '\0')
			return 1;

		if ((c < 'a' || c > 'z') && (c < 'A' && c > 'Z') && c != '_') {
			if (i == 0 || c < '0' || c > '9')
				return 0;
		}
	}

	return 0;
}

static uint8_t
get_process_from_pid(int pid, ProcessContext** process)
{
	if (pid < 0 || pid >= MAX_PROCESSES || processes[pid].stack_end == NULL)
		return 0;

	*process = &processes[pid];
	return 1;
}

int
proc_create(const ProcessCreateInfo* create_info)
{
	int pid = 0;
	for (; pid < MAX_PROCESSES && processes[pid].stack_end != NULL; pid++)
		continue;

	if (create_info->argc < 0 || pid == MAX_PROCESSES || !valid_name(create_info->name))
		return -1;

	void* stack_end = NULL;
	char* name = NULL;
	char** argv = NULL;

	if ((stack_end = mm_alloc(PROCESS_STACK_SIZE)) == NULL ||
	    (name = mm_alloc(strlen((char*)create_info->name) + 1)) == NULL ||
	    (create_info->argc != 0 && (argv = mm_alloc(sizeof(char*) * create_info->argc)) == NULL)) {
		mm_free(stack_end);
		mm_free(name);
		return -1;
	}

	strcpy(name, create_info->name);

	size_t len;
	for (int i = 0; i < create_info->argc; i++) {
		len = strlen(create_info->argv[i]) + 1;

		if ((argv[i] = mm_alloc(len)) == NULL) {
			mm_free(stack_end);
			mm_free(name);
			for (; i > 0; i--)
				mm_free(argv[i]);
			mm_free(argv);
			return -1;
		}

		memcpy(argv[i], create_info->argv[i], len);
	}

	ProcessContext* process = &processes[pid];
	memset(process, 0, sizeof(ProcessContext));

	process->stack_end = stack_end;
	process->stack_start = stack_end + PROCESS_STACK_SIZE;
	process->is_fg = create_info->is_fg;
	process->name = name;
	process->argv = argv;
	process->argc = create_info->argc;

	sch_on_process_create(pid,
	                      create_info->entry_point,
	                      create_info->priority,
	                      process->stack_start,
	                      create_info->argc,
	                      (const char* const*)argv);

	return pid;
}

int
proc_kill(int pid)
{
	ProcessContext* process;
	if (!get_process_from_pid(pid, &process))
		return -1;

	for (int i = 0; i < process->mem_count; i++)
		mm_free(process->mem[i]);
	mm_free(process->mem);

	sch_on_process_killed(pid);

	for (int i = 0; i < process->argc; i++)
		mm_free(process->argv[i]);
	mm_free(process->argv);
	mm_free(process->name);
	mm_free(process->stack_end);
	mm_free(process->stack_start);
	memset(process, 0, sizeof(ProcessContext));

	return 0;
}
