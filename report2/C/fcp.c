#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#define SIZE 1

double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

int cp(const char *read_name, const char *write_name, const size_t size)
{
	double tstart, tend;
	tstart = gettime();
	FILE *read_fp, *write_fp;

	if ((read_fp = fopen(read_name, "rb")) == NULL) {
		perror(NULL);
		fprintf(stderr, "ERROR : Failed to open \"%s\".\n", read_name);
		return EXIT_FAILURE;
	}
	if ((write_fp = fopen(write_name, "w")) == NULL) {
		perror(NULL);
		fprintf(stderr, "ERROR : Failed to open \"%s\".\n", write_name);
		return EXIT_FAILURE;
	}

	char buf[size];
	size_t get;
	int filled = 0;
	while(1) {
		char temp1[SIZE], temp2[SIZE];
		get = fread(temp1, 1, size - filled, read_fp);
		strcat(buf, temp1);
		if (get + filled == 0) break;
		int write_size = fwrite(buf, 1, get, write_fp);
		int i;
		for (i = write_size; i < get; i++) temp2[i - write_size] = buf[i];
		strcpy(buf, temp2);
		filled = get - write_size;
	}
	fclose(read_fp);
	fclose(write_fp);
	tend = gettime();
	printf("%f\n", tend - tstart);
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Wrong number of arguments.\n");
		exit(EXIT_FAILURE);
	}
	return cp(argv[1], argv[2], SIZE);
}
