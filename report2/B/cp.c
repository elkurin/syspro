#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#define SIZE 8192

double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

ssize_t cp(const char *read_pathname, const char *write_pathname)
{
	int read_fd = open(read_pathname, O_RDONLY);
	if (read_fd == - 1) {
		perror(NULL);
		printf("ERROR : Failed to open \"%s\".\n", read_pathname);
		return - 1;
	}
	
	if (!strcmp(read_pathname, write_pathname)) {
	if (close(read_fd) == - 1) {
		perror(NULL);
		printf("ERROR : Failed to close \"%s\".\n", read_pathname);
		return - 1;
	}
		return 0;
	}

	struct stat *read_stat;
	read_stat = (struct stat *)malloc(sizeof(struct stat));
	if (stat(read_pathname, read_stat) ==  - 1) {
		perror(NULL);
		printf("ERROR : Failed to get status of \"%s\".\n", read_pathname);
		return - 1;
	}
	int write_fd = open(write_pathname, O_CREAT|O_RDWR, read_stat->st_mode);
	// 0777
	if (write_fd == - 1) {
		perror(NULL);
		printf("ERROR : Failed to open \"%s\".\n", write_pathname);
		return - 1;
	}
	
	char buf[SIZE];
	while(1) {
		ssize_t get = read(read_fd, buf, SIZE);
		if (get == - 1) {
			perror(NULL);
			printf("ERROR : Failed to read.\n");
			return - 1;
		}
		if (get == 0) break;
		if (write(write_fd, buf, get) == - 1) {
			perror(NULL);
			printf("ERROR : Failed to write.\n");
			return - 1;
		}
		
	}

	if (close(write_fd) == - 1) {
		perror(NULL);
		printf("ERROR : Failed to close \"%s\".\n", write_pathname);
		return - 1;
	}

	if (close(read_fd) == - 1) {
		perror(NULL);
		printf("ERROR : Failed to close \"%s\".\n", read_pathname);
		return - 1;
	}

	return 0;
}

int main(void)
{
	double tstart, tend;
	tstart = gettime();
	if (cp("./a.txt", "./b.txt") == - 1) {
		printf("ERROR : Failed to copy.\n");
	}
	tend = gettime();
	printf("%f\n", tend - tstart);
	return 0;
}
