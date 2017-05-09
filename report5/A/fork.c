#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[], char *envp[])
{
	if (argc < 2) {
		printf("Not enough arguments.\n");
		exit(EXIT_FAILURE);
	}

	int i;
	pid_t pid = fork();
	int status;

	if (pid < 0) {
		perror("fork() failed\n");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		/* child process */
		char* newargv[argc];
		for (i = 1; i < argc; i++) {
			newargv[i - 1] = argv[i];
		}
		newargv[i] = NULL;
		execve(argv[1], newargv, envp);
		perror("execve() failed\n");
		exit(EXIT_FAILURE);
	} else {
		/* parent process */
		do {
			if (waitpid(pid, &status, WUNTRACED) < 0) {
				perror("waitpid() failed\n");
				exit(EXIT_FAILURE);
			}
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 0;
}

