#include <libc.h>
#include <memoryManager.h>
#include <pipe.h>
#include <process.h>
#include <queue.h>
#include <scheduler.h>

#define MAX_PIPES       32
#define BUFFER_MIN_SIZE 512
#define BUFFER_MAX_SIZE (1024 * 4)
#define PIPE_MAX_FD     (MAX_PIPES * 16)

typedef struct
{
	void* buffer;
	size_t size;
	size_t read_idx;
	size_t bytes_to_read;
	unsigned int readers, writers;
	Queue rd_q, wr_q;
	char* name;
} Pipe;

typedef struct
{
	int fd;
	int pid;
	int pipe_id;
	int allow_rd, allow_wr;
} PipeFd;

// utils
static size_t round_up_buffer_size(size_t size);

// getters
static PipeFd* get_pipe_fd(int pid, int fd);
static Pipe* get_pipe(PipeId pipe_id);

// find pipes
static PipeId find_named_pipe(const char* name);
static PipeId find_free_pipe(void);
static int find_free_pipe_fd(void);

// validators
static int is_name_valid(char* name);

// buffer manipulation
static size_t read_pipe_buffer(Pipe* pipe, char* buf, size_t count);
static size_t write_pipe_buffer(Pipe* pipe, char* buf, size_t count);

// callbacks
static int read_callback(int pid, int fd, char* buf, uint32_t count);
static int write_callback(int pid, int fd, char* buf, uint32_t count, uint32_t color);
static int close_callback(int pid, int fd);
static int dup_callback(int pid_from, int pid_to, int fd_from, int fd_to);
static void empty_fd_table(int pipe_id, int pid, int fd);
static Pipe* pipe_table[MAX_PIPES];
static PipeFd* pipe_fd_table[PIPE_MAX_FD];

PipeId
pipe_create()
{
	PipeId id = find_free_pipe();

	if (id == -1)
		return -1;

	Pipe* new_pipe = mm_alloc(sizeof(Pipe));
	Queue rd_q = queue_create();
	Queue wr_q = queue_create();

	if (new_pipe == NULL || rd_q == NULL || wr_q == NULL) {
		mm_free(new_pipe);
		if (rd_q != NULL)
			queue_free(rd_q);
		if (wr_q != NULL)
			queue_free(wr_q);
		return -1;
	}

	memset(new_pipe, 0, sizeof(Pipe));

	new_pipe->rd_q = rd_q;
	new_pipe->wr_q = wr_q;

	pipe_table[id] = new_pipe;

	return id;
}

int
pipe_free(PipeId id)
{
	Pipe* pipe = get_pipe(id);

	if (pipe == NULL)
		return -1;

	// empty all fd table with this pipe
	// empty_fd_table(id, -1);

	pipe_table[id] = NULL;

	mm_free(pipe->buffer);
	queue_free(pipe->wr_q);
	queue_free(pipe->rd_q);

	return 0;
}

PipeId
pipe_open(char* name)
{
	PipeId pipe_id = find_named_pipe(name);

	if (pipe_id == -1) {
		// lo creamos
		if (!is_name_valid(name))
			return -1;

		pipe_id = pipe_create();
		pipe_table[pipe_id]->name = name;
		if (pipe_id == -1)
			return -1;
	}

	return pipe_id;
}

static void
empty_fd_table(int pipe_id, int pid, int fd)
{
	//  we empty all fd table entries with this pipe_id, pid, fd, if fd ==-1 then we empty all fd table entries with
	//  this pipe_id, pid
	for (int i = 0; i < PIPE_MAX_FD; i++) {
		PipeFd* pipe_fd = pipe_fd_table[i];
		if (pipe_fd != NULL && pipe_fd->pipe_id == pipe_id && pipe_fd->pid == pid && (fd == -1 || pipe_fd->fd == fd)) {
			pipe_fd_table[i] = NULL;
			mm_free(pipe_fd);
		}
	}
}
int
pipe_unlink(char* name)
{
	PipeId pipe_id = find_named_pipe(name);
	if (pipe_id == -1)
		return -1;

	Pipe* pipe = get_pipe(pipe_id);

	pipe->name = NULL;

	if (pipe->readers == 0 && pipe->writers == 0) {
		return pipe_free(pipe_id);
	}

	if (pipe->readers == 0) {
		queue_unblock_all(pipe->wr_q);
	}
	if (pipe->writers == 0) {
		queue_unblock_all(pipe->rd_q);
	}
	return 0;
}

size_t
pipe_write(PipeId id, char* buf, size_t count)
{
	Pipe* pipe = get_pipe(id);
	if (pipe == NULL)
		return -1;

	return write_pipe_buffer(pipe, buf, count);
}

size_t
pipe_read(PipeId id, char* buf, size_t count)
{
	Pipe* pipe = get_pipe(id);
	if (pipe == NULL)
		return -1;

	return read_pipe_buffer(pipe, buf, count);
}

int
pipe_map_fd(int pid, int fd, PipeId pipe_id, int allow_rd, int allow_wr)
{
	Pipe* pipe = get_pipe(pipe_id);

	if (pipe == NULL)
		return -1;

	int pipe_fd_idx = find_free_pipe_fd();

	if (pipe_fd_idx == -1)
		return -1;

	PipeFd* pipe_fd = mm_alloc(sizeof(PipeFd));

	pipe_fd->pipe_id = pipe_id;
	pipe_fd->allow_rd = allow_rd;
	pipe_fd->allow_wr = allow_wr;
	pipe_fd->pid = pid;

	pipe_fd_table[pipe_fd_idx] = pipe_fd;

	if (allow_rd)
		pipe->readers++;
	if (allow_wr)
		pipe->writers++;

	fd = proc_map_fd(pid, fd, read_callback, write_callback, close_callback, dup_callback);
	pipe_fd->fd = fd;

	return fd;
}

int
pipe_status(PipeStatus* status_arr, int max_size)
{
	int i;
	int size = 0;
	for (i = 0; i < MAX_PIPES && size < max_size; i++) {
		if (pipe_table[i] != NULL) {
			Pipe* pipe = pipe_table[i];
			PipeStatus status;
			status.pipe_id = i;
			status.readers = pipe->readers;
			status.writers = pipe->writers;
			status.bytes_to_read = pipe->bytes_to_read;
			status_arr[size++] = status;
		}
	}
	return size;
}

static size_t
round_up_buffer_size(size_t size)
{
	size--;
	size |= size >> 1;
	size |= size >> 2;
	size |= size >> 4;
	size |= size >> 8;
	size |= size >> 16;
	size++;
	return size;
}

static PipeFd*
get_pipe_fd(int pid, int fd)
{
	PipeFd* pipe_fd;
	for (int i = 0; i < PIPE_MAX_FD; i++) {
		pipe_fd = pipe_fd_table[i];
		if (pipe_fd != NULL && pipe_fd->pid == pid && pipe_fd->fd == fd)
			return pipe_fd;
	}
	return NULL;
}

static Pipe*
get_pipe(PipeId pipe_id)
{
	if (pipe_id >= 0 && pipe_id < MAX_PIPES)
		return pipe_table[pipe_id];
	return NULL;
}

static PipeId
find_named_pipe(const char* name)
{
	if (name == NULL)
		return -1;

	for (int i = 0; i < MAX_PIPES; i++) {
		if (pipe_table[i] != NULL && pipe_table[i]->name != NULL && strcmp(pipe_table[i]->name, name) == 0)
			return i;
	}
	return -1;
}

static PipeId
find_free_pipe(void)
{
	int i;
	for (i = 0; i < MAX_PIPES; i++) {
		if (pipe_table[i] == NULL)
			return i;
	}
	return -1;
}

static int
find_free_pipe_fd(void)
{
	int i;
	for (i = 0; i < PIPE_MAX_FD; i++) {
		if (pipe_fd_table[i] == NULL)
			return i;
	}
	return -1;
}

static int
is_name_valid(char* name)
{
	// The first character must be a letter or a slash. Subsequent characters may be a letter or a number.
	if (name == NULL || strlen(name) > MAX_NAME_LENGHT || !isalpha(name[0]))
		return 0;

	for (int i = 1; i < strlen(name); i++) {
		if (!isalnum(name[i]))
			return 0;
	}
	return 1;
}

static size_t
read_pipe_buffer(Pipe* pipe, char* buf, size_t count)
{
	if (pipe->bytes_to_read == 0)
		return 0;

	size_t read_idx = pipe->read_idx;
	count = count < pipe->bytes_to_read ? count : pipe->bytes_to_read;

	if (read_idx + count <= pipe->size) {
		memcpy(buf, pipe->buffer + read_idx, count);
	} else {
		size_t first_part = pipe->size - read_idx;
		memcpy(buf, pipe->buffer + read_idx, first_part);
		memcpy(buf + first_part, pipe->buffer, count - first_part);
	}

	pipe->read_idx = (read_idx + count) % pipe->size;
	pipe->bytes_to_read -= count;

	// Desbloqueamos a todos los escritores
	queue_unblock_all(pipe->wr_q);

	return count;
}

static size_t
write_pipe_buffer(Pipe* pipe, char* buf, size_t count)
{
	size_t required = pipe->bytes_to_read + count;

	if (pipe->size < required && pipe->size < BUFFER_MAX_SIZE) {
		// Amplíamos el tamaño del buffer

		size_t new_size = required < BUFFER_MAX_SIZE ? required : BUFFER_MAX_SIZE;
		new_size = round_up_buffer_size(new_size);
		void* new_buf = mm_alloc(new_size);

		if (new_buf == NULL)
			return -1;

		size_t bytes_left = pipe->size - pipe->read_idx;

		// como el buffer es circular, podríamos tener que leer en 2 partes.
		if (bytes_left >= pipe->bytes_to_read) {
			memcpy(new_buf, pipe->buffer + pipe->read_idx, pipe->bytes_to_read);
		} else {
			memcpy(new_buf, pipe->buffer + pipe->read_idx, bytes_left);
			memcpy(new_buf + bytes_left, pipe->buffer, pipe->bytes_to_read - bytes_left);
		}
		pipe->read_idx = 0;
		mm_free(pipe->buffer);
		pipe->buffer = new_buf;
		pipe->size = new_size;
	}

	if (pipe->bytes_to_read == 0)
		pipe->read_idx = 0;

	size_t write_idx = (pipe->read_idx + pipe->bytes_to_read) % pipe->size;
	size_t avail_space = pipe->size - pipe->bytes_to_read;

	count = count < avail_space ? count : avail_space;

	if (count == 0)
		return 0;

	if (write_idx + count <= pipe->size) {
		memcpy(pipe->buffer + write_idx, buf, count);
	} else {
		size_t first_part = pipe->size - write_idx;
		memcpy(pipe->buffer + write_idx, buf, first_part);
		memcpy(pipe->buffer, buf + first_part, count - first_part);
	}

	pipe->bytes_to_read += count;

	// Desbloqueamos a todos los lectores
	queue_unblock_all(pipe->rd_q);

	return count;
}

static int
read_callback(int pid, int fd, char* buf, uint32_t count)
{
	PipeFd* pipe_fd = get_pipe_fd(pid, fd);
	Pipe* pipe = get_pipe(pipe_fd->pipe_id);

	size_t bytes_read = 0;

	while ((bytes_read = read_pipe_buffer(pipe, buf + bytes_read, count)) == 0 &&
	       (pipe->name != NULL || pipe->writers > 0)) {
		queue_add(pipe->rd_q, pid);
		sch_block(pid);
		sch_yield();
	}

	return bytes_read;
}

static int
write_callback(int pid, int fd, char* buf, uint32_t count, uint32_t color)
{
	PipeFd* pipe_fd = get_pipe_fd(pid, fd);
	Pipe* pipe = get_pipe(pipe_fd->pipe_id);

	size_t bytes_written = 0;

	while (bytes_written < count) {
		size_t written = write_pipe_buffer(pipe, buf + bytes_written, count - bytes_written);

		if (written == 0) {
			// Si no hay espacio para escribir, bloqueamos al proceso
			// y lo ponemos en la cola de escritores
			queue_add(pipe->wr_q, pid);
			sch_block(pid);
			sch_yield();
		} else {
			bytes_written += written;
		}
	}

	return bytes_written;
}

static int
close_callback(int pid, int fd)
{
	PipeFd* pipe_fd = get_pipe_fd(pid, fd);
	Pipe* pipe = get_pipe(pipe_fd->pipe_id);

	if (pipe_fd->allow_rd) {
		pipe->readers--;
		empty_fd_table(pipe_fd->pipe_id, pid, fd);
		if (pipe->readers == 0)
			queue_unblock_all(pipe->wr_q);
	}

	if (pipe_fd->allow_wr) {
		pipe->writers--;
		empty_fd_table(pipe_fd->pipe_id, pid, fd);
		if (pipe->writers == 0)
			queue_unblock_all(pipe->rd_q);
	}

	if (pipe->readers == 0 && pipe->writers == 0) {
		empty_fd_table(pipe_fd->pipe_id, pid, -1);
		if (pipe->bytes_to_read == 0)
			pipe_free(pipe_fd->pipe_id);
		pipe_fd_table[fd] = NULL;
		mm_free(pipe_fd);
	}

	return 0;
}

static int
dup_callback(int pid_from, int pid_to, int fd_from, int fd_to)
{
	PipeFd* pipe_fd = get_pipe_fd(pid_from, fd_from);
	if (pipe_fd == NULL)
		return -1;

	Pipe* pipe = get_pipe(pipe_fd->pipe_id);
	if (pipe == NULL)
		return -1;

	return pipe_map_fd(pid_to, fd_to, pipe_fd->pipe_id, pipe_fd->allow_rd, pipe_fd->allow_wr);
}
