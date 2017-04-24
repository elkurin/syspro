#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main(void)
{
	double tstart, tend;
	pid_t p;
	
	tstart = gettime();
	p = getpid();
	tend = gettime();

	printf("%f\n", tend - tstart);

	tstart = gettime();
	int i;
	for (i = 0; i < 1000; i++) p = getpid();
	tend = gettime();

	printf("%f\n", tend - tstart);

	return 0;
}
