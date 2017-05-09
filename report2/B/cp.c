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
	struct stat *write_stat;
	write_stat = (struct stat *)malloc(sizeof(struct stat));
	if (stat(write_pathname, write_stat) ==  - 1) {
		perror(NULL);
		printf("ERROR : Failed to get status of \"%s\".\n", write_pathname);
		return - 1;
	}

	if (read_stat->st_ino == write_stat->st_ino) {
	if (close(read_fd) == - 1) {
		perror(NULL);
		printf("ERROR : Failed to close \"%s\".\n", read_pathname);
		return - 1;
	}
		printf("same path.\n");
		return 0;
	}

	char buf[SIZE];
	int filled = 0;
	while(1) {
		char temp1[SIZE];
		ssize_t get = read(read_fd, temp1, SIZE - filled);
		if (get == - 1) {
			perror(NULL);
			printf("ERROR : Failed to read.\n");
			return - 1;
		}
		if (get + filled == 0) break;
		strcat(buf, temp1);
		int write_size = write(write_fd, buf, get);
		if (write_size == - 1) {
			perror(NULL);
			printf("ERROR : Failed to write.\n");
			return - 1;
		}
		char temp2[SIZE];
		int i;
		for (i = write_size; i < get; i++) temp2[i - write_size] = buf[i];
		strcpy(buf, temp2);
		filled = get - write_size;
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

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Wrong number of arguments.\n");
		exit(EXIT_FAILURE);
	}
	double tstart, tend;
	tstart = gettime();
	if (cp(argv[1], argv[2]) == - 1) {
		printf("ERROR : Failed to copy.\n");
	}
	tend = gettime();
	printf("%f\n", tend - tstart);
	return 0;
}
