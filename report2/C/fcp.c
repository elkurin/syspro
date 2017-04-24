#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

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
	while(1) {
		get = fread(buf, 1, size, read_fp);
		if (get == 0) break;
		fwrite(buf, 1, get, write_fp);
	}
	fclose(read_fp);
	fclose(write_fp);
	tend = gettime();
	printf("%f\n", tend - tstart);
	return EXIT_SUCCESS;
}

int main(void)
{
	return cp("hoge.txt", "foo.txt", 1);
}
