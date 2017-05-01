btree: btree.c btree.h
	gcc -Wall -o btree btree.c -lpthread

btree1: btree1.c btree.h
	gcc -Wall -o btree1 btree1.c -lpthread

lock: lock_btree.c btree.h
	gcc -Wall -o lock lock_btree.c -lpthread

clean:
	rm -f btree btree1 lock
