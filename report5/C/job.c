#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "./parser/parse.h"

int main(void)
{
	job *cur_job;
	char s[LINELEN];
	while (get_line(s, LINELEN)) {
		if (!strcmp(s, "exit\n")) return 0;
		cur_job = parse_line(s);

		while(cur_job != NULL) {
			process* pr = cur_job->process_list;
			int fd[2];
			pipe(fd);
			int pid1, pid2 = - 2;
			int out_fd = - 1, in_fd = - 1;
			int status;
			if ((pid1 = fork()) == 0) {
				/* child process */

				char* argv[ARGLSTLEN];
				char* envp[] = {NULL};
				if (pr->program_name == NULL) {
					printf("program is empty\n");
					exit(EXIT_FAILURE);
				}
				argv[0] = (char *)malloc(100);
				strcpy(argv[0], pr->program_name);
				int i;
				if (pr->argument_list != NULL) {
					for (i = 0; pr->argument_list[i] != NULL; i++) {
						argv[i] = (char *)malloc(100);
						strcpy(argv[i], pr->argument_list[i]);
					}
					argv[i] = NULL;
				}

				if (pr->input_redirection != NULL) {
					in_fd = open(pr->input_redirection, O_RDWR);
					if (in_fd < 0) {
						perror("open() failed\n");
						exit(EXIT_FAILURE);
					}
					dup2(in_fd, 0);
				}

				if (pr->output_redirection != NULL) {
					out_fd = open(pr->output_redirection, O_CREAT|O_RDWR, 0666);
					if (out_fd < 0) {
						perror("open() failed\n");
						exit(EXIT_FAILURE);
					}
					dup2(out_fd, 1);
				}

				if (pr->next != NULL) dup2(fd[1], 1);
				
				execve(argv[0], argv, envp);
				perror("execve() pid1 failed\n");
				exit(EXIT_FAILURE);
			} else if (pid1 == - 1) {
				perror("fork() failed\n");
				exit(EXIT_FAILURE);
			} else {
				/* parent process */
				close(fd[1]);
				do {
					if (waitpid(pid1, &status, WUNTRACED) < 0) {
						perror("waitpid() failed\n");
						exit(EXIT_FAILURE);
					}
				} while(!WIFEXITED(status) && !WIFSIGNALED(status));
			}

			if (pr->next != NULL) {
				if ((pid2 = fork()) == 0) {
					pr = pr->next;
					char* argv[ARGLSTLEN];
					char* envp[] = {NULL};
					if (pr->program_name == NULL) {
						printf("program is empty\n");
						exit(EXIT_FAILURE);
					}
					argv[0] = (char *)malloc(100);
					strcpy(argv[0], pr->program_name);
					int i;
					if (pr->argument_list != NULL) {
						for (i = 0; pr->argument_list[i] != NULL; i++) {
							argv[i] = (char *)malloc(100);
							strcpy(argv[i], pr->argument_list[i]);
						}
						argv[i] = NULL;
					}

					if (pr->input_redirection != NULL) {
						in_fd = open(pr->input_redirection, O_RDWR);
						if (in_fd < 0) {
							perror("open() failed\n");
							exit(EXIT_FAILURE);
						}
						dup2(in_fd, 0);
					}
					if (pr->output_redirection != NULL) {
						out_fd = open(pr->output_redirection, O_CREAT|O_RDWR, 0666);
						if (out_fd < 0) {
							perror("open() failed\n");
							exit(EXIT_FAILURE);
						}
						dup2(out_fd, 1);
					}
					dup2(fd[0], 0);

					execve(argv[0], argv, envp);
					perror("execve() pid2 failed\n");
					exit(EXIT_FAILURE);
				} else if (pid2 == - 1) {
				   perror("fork() failed\n");
				   exit(EXIT_FAILURE);
				} else {
					/* parent process */
					close(fd[0]);
					do {
						if (waitpid(pid2, &status, WUNTRACED) < 0) {
							perror("waitpid() failed\n");
							exit(EXIT_FAILURE);
						}
					} while(!WIFEXITED(status) && !WIFSIGNALED(status));
				}

			}
			cur_job = cur_job->next;
		}
	}
	return 0;
}

