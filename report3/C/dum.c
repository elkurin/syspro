#define _REENTRANT

#include <stdio.h>
#include <pthread.h>

void *func(void *arg)
{
	pthread_exit(NULL);
}

int main(void)
{
	pthread_t thread[10];
	int dummy = - 1;

	int i;

	return 0;
}
