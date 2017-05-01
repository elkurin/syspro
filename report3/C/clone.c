#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/utsname.h>

static int childFunc(void *arg)
{
	struct utsname uts;

	if (sethostname(arg, strlen(arg)) == - 1) {
		perror("set host name");
		exit(EXIT_FAILURE);
	}

	if (uname(&uts) == - 1) {
		perror("uname");
		exit(EXIT_FAILURE);
	}

	printf("uts.nodename in child : %s\n", uts.nodename);

	sleep(200);
	return 0;
}

#define STACK_SIZE (1024 * 1024)

int main(int argc, char *argv[])
{
	char *stack;
	char *stackTop;
	pid_t pid;
	struct utsname uts;

	if (argc < 2) {
		fprintf(stderr, "Usage : %s <child-hostname>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	stack = malloc(STACK_SIZE);
	if (stack == NULL) {
		perror("malloc stack");
		exit(EXIT_FAILURE);
	}
	stackTop = stack + STACK_SIZE;

	pid = clone(childFunc, stackTop, CLONE_NEWUTS | SIGCHLD, argv[1]);
	if (pid == - 1) {
		perror("clone");
		exit(EXIT_FAILURE);
	}
	printf("clone() returned %ld\n", (long) pid);

	sleep(1);

	if (uname(&uts) == - 1) {
		perror("uname");
		exit(EXIT_FAILURE);
	}

	if (waitpid(pid, NULL, 0) == - 1) {
		perror("waitpid");
		exit(EXIT_FAILURE);
	}
	printf("child has terminated\n");

	exit(EXIT_SUCCESS);
}
