#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <tests.h>

#define PIPE_NAME "testpipe"

extern Color color;

void
reader()
{
	int pid = asm_getpid();

	printf_color(color.output, "Reading process with pid: %d \n", pid);

	int pipe_fd[2];

	if (asm_pipe_open(PIPE_NAME, pipe_fd) < 0) {
		printf_color(color.error, "ERROR opening pipe \n");
		return;
	}

	char buffer[101] = { 0 };
	size_t count = asm_read(pipe_fd[0], buffer, 100);
	buffer[count] = '\0';
	printf_color(color.output, "Process %d read from pipe \"%s\" the content is \"%s\" \n", pid, PIPE_NAME, buffer);
	asm_pipe_unlink(PIPE_NAME);
	printf_color(color.output, "unlinking pipe... \n");
}

void
writer()
{
	int pid = asm_getpid();

	printf_color(color.output, "Writing process with pid: %d \n", pid);

	int pipe_fd[2];
	printf_color(color.output, "Process %d opening pipe \"%s\" \n", pid, PIPE_NAME);
	if (asm_pipe_open(PIPE_NAME, pipe_fd) < 0) {
		fputs(STDERR, "ERROR opening pipe \n", color.error);
		return;
	}
	printf_color(color.output, "writing to fd: %d \n", pipe_fd[1]);

	fputs(pipe_fd[1], "This message comes from a dead process", color.output);
}

int
test_pipes(int argc, char* argv[])
{
	int p = asm_getpid();
	printf_color(color.output, "Main process with pid %d created \n", p);

	ProcessCreateInfo pci = {
		.name = "writeProcess", .is_fg = 1, .priority = 0, .entry_point = (EntryPoint)writer, .argc = 0, .argv = NULL
	};

	printf_color(color.output, "Process %d creating the process that will open and write the pipe \n", p);
	int wpid = asm_execve(&pci);
	if (wpid < 0) {
		printf_color(color.error, "Error while trying to create the writing process \n");
		return 0;
	}

	asm_waitpid(wpid);
	printf_color(color.output, "Process %d finished \n", wpid);

	ProcessCreateInfo pci2 = {
		.name = "readingProcess", .is_fg = 1, .priority = 0, .entry_point = (EntryPoint)reader, .argc = 0, .argv = NULL
	};

	printf_color(color.output, "\nProcess %d creating the process that will read and close the pipe \n", p);

	int rpid = asm_execve(&pci2);
	if (rpid < 0) {
		printf_color(color.error, "Error while trying to create the reading process \n");
		return 0;
	}

	asm_waitpid(rpid);
	printf_color(color.output, "Process %d finished.\n", rpid);
	return 0;
}
