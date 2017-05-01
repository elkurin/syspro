#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define pthread_num 100

double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

typedef struct _tnode {
	int val;
	struct _tnode *l;
	struct _tnode *r;
} tnode;

tnode *btree_create()
{
	return NULL;
}

tnode *btree_insert(int v, tnode *t)
{
	if (t == NULL) {
		t = (tnode *)malloc(sizeof(tnode));
		t->val = v;
		t->l = NULL;
		t->r = NULL;
	} else {
		if (v < t->val) t->l = btree_insert(v, t->l);
		else if (v > t->val) t->r = btree_insert(v, t->r);
	}
	return t;
}

void btree_destroy(tnode *t)
{
	if (t == NULL) return;
	btree_destroy(t->l);
	btree_destroy(t->r);
	free(t);
}

void btree_dump(tnode *t)
{
	if (t == NULL) return;
	btree_dump(t->l);
	printf("%d\n", t->val);
	btree_dump(t->r);
}

tnode *tree;
int next;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *func(void *arg)
{
	int i;
	for (i = 0; i < 100; i++) {
		pthread_mutex_lock(&mutex);
		tree = btree_insert(next, tree);
		next++;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

int main(void)
{
	pthread_t thread[pthread_num];

	tree = btree_create();
	next = 0;

	int i;
	double tstart = gettime();
	for (i = 0; i < pthread_num; i++) {
		if (pthread_create(&thread[i], NULL, func, &i) != 0) {
			printf("ERROR : pthread_create\n");
			return - 1;
		}
	}
	for (i = 0; i < pthread_num; i++) {
		if (pthread_join(thread[i], NULL) != 0) {
			printf("ERROR : pthread_join\n");
			return - 1;
		}
	}

	double tend = gettime();
	btree_dump(tree);
	btree_destroy(tree);
	pthread_mutex_destroy(&mutex);
	printf("%f\n", tend - tstart);

	return 0;
}
