#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <tests.h>

#define pipe_name "testpipe"
void
reader()
{
	int pid = asm_getpid();

	printf("Reading process with pid: %d \n", pid);

	int pipe_fd[2];

	if (asm_pipe_open(pipe_name, pipe_fd) < 0) {
		printf("ERROR opening pipe \n");
		return;
	}

	char buffer[101] = { 0 };
	size_t count = asm_read(pipe_fd[0], buffer, 100);
	buffer[count] = '\0';
	printf("Process %d read from pipe \"%s\" the content te \"%s\" \n", pid, pipe_name, buffer);
	asm_pipe_unlink(pipe_name);
	printf("unlinking pipe... \n");
}

void
writer()
{
	int pid = asm_getpid();

	printf("Writing process with pid: %d \n", pid);

	int pipe_fd[2];
	printf("Process %d opening pipe \"%s\" \n", pid, pipe_name);
	if (asm_pipe_open(pipe_name, pipe_fd) < 0) {
		fputs(STDOUT, "ERROR opening pipe \n", WHITE);
		return;
	}
	printf("writing to fd: %d \n", pipe_fd[1]);

	fputs(pipe_fd[1], "This message comes from a dead process", WHITE);
}

int
test_pipes(int argc, char* argv[])
{
	int p = asm_getpid();
	printf("Main process with pid %d created \n", p);

	ProcessCreateInfo pci = {
		.name = "writeProcess", .is_fg = 1, .priority = 0, .entry_point = (EntryPoint)writer, .argc = 0, .argv = NULL
	};

	printf("Process %d creating the process that will open and write the pipe \n", p);
	int wpid = asm_execve(&pci);
	if (wpid < 0) {
		printf("Error while trying to create the writing process \n");
		return 0;
	}

	asm_waitpid(wpid);
	printf("Process %d finished \n", wpid);

	ProcessCreateInfo pci2 = {
		.name = "readingProcess", .is_fg = 1, .priority = 0, .entry_point = (EntryPoint)reader, .argc = 0, .argv = NULL
	};

	printf("\nProcess %d creating the process that will read and close the pipe \n", p);

	int rpid = asm_execve(&pci2);
	if (rpid < 0) {
		printf("Error while trying to create the reading process \n");
		return 0;
	}

	asm_waitpid(rpid);
	printf("Process %d finished.\n", rpid);
	return 0;
}
