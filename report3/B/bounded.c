#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 10
#define thread_num 100
#define EMPTY -1

int bb_buf[SIZE];
int bb_get(void);
void bb_put(int i);
int fore = 0;
int last = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t read = PTHREAD_COND_INITIALIZER;
pthread_cond_t written = PTHREAD_COND_INITIALIZER;

void bb_put(int i)
{
	pthread_mutex_lock(&mutex);
	while (bb_buf[last] != EMPTY) {
		//printf("wait put %d %d\n", fore, last);
		pthread_cond_wait(&read, &mutex);
	}
	//printf("put %d %d\n", fore, last);
	bb_buf[last] = i;
	last++;
	if (last == SIZE) last = 0;
	pthread_cond_signal(&written);
	pthread_mutex_unlock(&mutex);
}

int bb_get(void)
{
	pthread_mutex_lock(&mutex);
	while (bb_buf[fore] == EMPTY) {
		//printf("wait get %d %d\n", fore, last);
		pthread_cond_wait(&written, &mutex);
	}
	//printf("get %d %d\n", fore, last);
	int ret = bb_buf[fore];
	bb_buf[fore] = EMPTY;
	fore++;
	if (fore == SIZE) fore = 0;
	pthread_cond_signal(&read);
	pthread_mutex_unlock(&mutex);
	return ret;
}

void *func1(void *arg)
{
	int i;
	for (i = 0; i < 100; i++) bb_put(i);
	pthread_exit(NULL);
}
	
void *func2(void *arg)
{
	int i;
	for (i = 0; i < 100; i++) printf("%d\n", bb_get());
	pthread_exit(NULL);
}
	
int main(void)
{
	int i;
	for (i = 0; i < SIZE; i++) bb_buf[i] = EMPTY;
	int dummy = - 1;
	pthread_t thread1[thread_num], thread2[thread_num];
	for (i = 0;  i < thread_num; i++) {
		if (pthread_create(&thread1[i], NULL, func1, &dummy) != 0) {
			printf("ERROR : pthread_create thread1\n");
			return - 1;
		}
		if (pthread_create(&thread2[i], NULL, func2, &dummy) != 0) {
			printf("ERROR : pthread_create thread2\n");
			return - 1;
		}
	}

	for (i = 0; i < thread_num; i++) {
		if (pthread_join(thread1[i], NULL) != 0) {
			printf("ERROR : pthread_join thread1\n");
			return - 1;
		}
		if (pthread_join(thread2[i], NULL) != 0) {
			printf("ERROR : pthread_join thread2\n");
			return - 1;
		}
	}
	
	return 0;
}
