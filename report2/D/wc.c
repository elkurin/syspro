#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 8192

int wc(const char *pathname)
{
	int fd = open(pathname, O_RDONLY);
	if (fd == - 1) {
		perror(NULL);
		printf("ERROR : Failed to open \"%s\".\n", pathname);
		return - 1;
	}

	char buf[SIZE];
	int line = 0, word = 0,byte = 0;
	int prev = 1;

	while(1) {
		ssize_t get = read(fd, buf, SIZE);
		if (get == - 1) {
			perror(NULL);
			printf("ERROR : Failed to read \"%s\".\n", pathname);
			return - 1;
		}
		if (get == 0) break;
		byte += get;
		int i;
		for (i = 0; i < get; i++) {
			if (buf[i] == '\n') line++;
			if (buf[i] == '\n' || buf[i] == ' ' || buf[i] == '\t') {
				if (!prev) word++;
				prev = 1;
			} else prev = 0;
		}
	}
	printf("%d %d %d\n", line, word, byte);
	return 0;
}

int main(void)
{
	return wc("foo.txt");
}
