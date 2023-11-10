#include <libc.h>
#include <memoryManager.h>
#include <process.h>
#include <queue.h>
#include <scheduler.h>
#include <text.h>

typedef struct
{
	ReadCallback read_callback;
	WriteCallback write_callback;
	CloseCallback close_callback;
	DupCallback dup_callback;
} FileDescriptor;

typedef struct
{
	void *stack_end, *stack_start;
	uint8_t is_fg;
	char* name;

	int argc;
	char** argv;

	FileDescriptor fds[MAX_FDS];
	Queue waiting_pids;
	Queue child_pids;
} ProcessContext;

static ProcessContext processes[MAX_PROCESSES];

static uint8_t valid_name(const char* name);
static uint8_t get_process_from_pid(int pid, ProcessContext** process);

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
		mm_free(argv);
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

	int parent_pid = sch_get_current_pid();
	if (parent_pid >= 0) {
		ProcessContext* parent_proc = &processes[parent_pid];

		if (parent_proc->child_pids == NULL && (parent_proc->child_pids = queue_create()) == NULL) {
			proc_kill(pid, -1);
			return -1;
		}
		if (queue_add(parent_proc->child_pids, pid) == -1) {
			proc_kill(pid, -1);
			return -1;
		}
	}

	return pid;
}

int
proc_kill(int pid, uint8_t status)
{
	ProcessContext* process;
	if (!get_process_from_pid(pid, &process))
		return -1;

	sch_on_process_killed(pid, status);

	int child_pid;
	while ((child_pid = queue_pop(process->child_pids)) != -1)
		proc_kill(child_pid, -1);
	mm_free(process->child_pids);

	if (process->waiting_pids != NULL)
		queue_free(process->waiting_pids);

	for (int i = 0; i < process->argc; i++)
		mm_free(process->argv[i]);

	for (int i = 0; i < MAX_FDS; i++) {
		if (process->fds[i].close_callback != NULL)
			process->fds[i].close_callback(pid, i);
		process->fds[i].read_callback = NULL;
		process->fds[i].write_callback = NULL;
		process->fds[i].close_callback = NULL;
		process->fds[i].dup_callback = NULL;
	}

	mm_free(process->argv);
	mm_free(process->name);
	mm_free(process->stack_end);
	memset(process, 0, sizeof(ProcessContext));

	return 0;
}

int
proc_unblock_on_killed(int waiting_pid, int working_pid)
{
	ProcessContext* process;
	if (!get_process_from_pid(working_pid, &process))
		return -1;
	if (process->waiting_pids == NULL && (process->waiting_pids = queue_create()) == NULL)
		return -1;
	queue_add(process->waiting_pids, waiting_pid);
	return 0;
}

int
proc_list(Process* procs, int max_procs)
{
	int count = 0;
	ProcessContext* context;

	for (int i = 0; i < MAX_PROCESSES && count < max_procs; i++) {
		context = &processes[i];

		if (context->stack_end != NULL) {
			procs[count].pid = i;
			strcpy(procs[count].name, context->name);
			procs[count].stack_end = context->stack_end;
			procs[count].stack_start = context->stack_start;
			procs[count].is_fg = context->is_fg;
			sch_get_proc_info(i, &procs[count]);

			count++;
		}
	}

	return count;
}

int
proc_map_fd(int pid,
            int fd,
            ReadCallback read_callback,
            WriteCallback write_callback,
            CloseCallback close_callback,
            DupCallback dup_callback)
{
	ProcessContext* process;
	if (!get_process_from_pid(pid, &process) || fd > MAX_FDS)
		return -1;

	if (fd < 0) {
		for (fd = 3; process->fds[fd].write_callback != NULL && process->fds[fd].read_callback != NULL; fd++)
			continue;
	}

	process->fds[fd].read_callback = read_callback;
	process->fds[fd].write_callback = write_callback;
	process->fds[fd].close_callback = close_callback;
	process->fds[fd].dup_callback = dup_callback;
	return fd;
}

int
proc_unmap_fd(int pid, int fd)
{
	ProcessContext* process;
	if (!get_process_from_pid(pid, &process) || fd > MAX_FDS || fd < 0)
		return -1;
	int r = 0;
	if (process->fds[fd].close_callback != NULL && (r = process->fds[fd].close_callback(pid, fd)) != 0)
		return r;
	process->fds[fd].read_callback = NULL;
	process->fds[fd].write_callback = NULL;
	process->fds[fd].close_callback = NULL;
	process->fds[fd].dup_callback = NULL;
	return r;
}

int
proc_read(int pid, int fd, char* buf, uint32_t size)
{
	ProcessContext* process;
	if (fd < 0 || fd > MAX_FDS || !get_process_from_pid(pid, &process) || process->fds[fd].read_callback == NULL)
		return -1;
	return process->fds[fd].read_callback(pid, fd, buf, size);
}

int
proc_write(int pid, int fd, char* buf, uint32_t size, uint32_t color)
{
	ProcessContext* process;
	if (fd < 0 || fd > MAX_FDS || !get_process_from_pid(pid, &process) || process->fds[fd].write_callback == NULL)
		return -1;
	return process->fds[fd].write_callback(pid, fd, buf, size, color);
}

int
proc_dup(int pid, int fd_original, int fd_final)
{
	ProcessContext* process;
	if (fd_original < 0 || fd_original > MAX_FDS || fd_final < 0 || fd_final > MAX_FDS ||
	    !get_process_from_pid(pid, &process))
		return -1;

	ReadCallback raux = process->fds[fd_original].read_callback;
	WriteCallback waux = process->fds[fd_original].write_callback;
	CloseCallback caux = process->fds[fd_original].close_callback;
	DupCallback daux = process->fds[fd_original].dup_callback;

	process->fds[fd_original].read_callback = process->fds[fd_final].read_callback;
	process->fds[fd_original].write_callback = process->fds[fd_final].write_callback;
	process->fds[fd_original].close_callback = process->fds[fd_final].close_callback;
	process->fds[fd_original].dup_callback = process->fds[fd_final].dup_callback;

	process->fds[fd_final].read_callback = raux;
	process->fds[fd_final].write_callback = waux;
	process->fds[fd_final].close_callback = caux;
	process->fds[fd_final].dup_callback = daux;

	return 0;
}

int
proc_is_fg(int pid)
{
	ProcessContext* process;
	if (!get_process_from_pid(pid, &process))
		return -1;
	return process->is_fg;
}

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
