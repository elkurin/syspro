#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define pthread_num 100

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

void *func(void *arg)
{
	int *p = (int *)arg;
	tree = btree_insert(*p, tree);
	pthread_exit(NULL);
}


int main(void)
{
	pthread_t thread[pthread_num];

	tree = btree_create();

	int i, j;
	for (j = 0; j < 100; j++) {
		for (i = 0; i < pthread_num; i++) {
			int give = j * pthread_num + i;
			if (pthread_create(&thread[i], NULL, func, &give) != 0) {
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
	}


	btree_dump(tree);
	btree_destroy(tree);

	return 0;
}
