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
	for (i = 0; i < 10; i++) {
		if (pthread_create(&thread[i], NULL, func, &dummy) != 0) {
			printf("ERROR : pthread_create\n");
			return - 1;
		}
	}
	for (i = 0; i < 10; i++) {
		if (pthread_join(thread[i], NULL) != 0) {
			printf("ERROR : pthread_join\n");
			return - 1;
		}
	}

	return 0;
}
