#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./parse/parse.h"

int main(void)
{
	job *cur_job;
	char s[LINELEN];
	get_line(s, LINELEN);
	if (!strcmp(s, "exit\n")) return 0;
	cur_job = parse_line(s);

	while(cur_job != NULL) {
		pid_t pid = fork();
		int status;
		if (pid < 0) {
			perror("fork() failed\n");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
			/* child process */

			char* argv[ARGLSTLEN];
			char** envp;
			process* pr = cur_job->process_list;
			if (pr->program_name == NULL) {
				printf("program is empty\n");
				exit(EXIT_FAILURE);
			}
			strcpy(argv[0], pr->program_name);
			if (pr->argument_list != NULL) {
				int i;
				for (i = 0; pr->argument_list[i] != NULL; i++) strcpy(argv[i + 1], pr->argument_list[i]);
				argv[i + 1] = NULL;
			}
			execve(argv[0], argv, envp);
			perror("execve() failed\n");
			exit(EXIT_FAILURE);
		} else {
			do {
				if (waitpid(pid, &status, WUNTRACED) < 0) {
					perror("waitpid() failed\n");
					exit(EXIT_FAILURE);
				}
			} while(!WIFEXITED(status) && !I|WIFSIGNALED(status));
		}
		cur_job = cur_job->next;
	}
	return 0;
}

