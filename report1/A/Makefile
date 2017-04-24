### Create kadai-a from 3 files ###

kadai-a: a.o b.o c.o
	gcc a.o b.o c.o -o kadai-a

a.o: a.c
	gcc -Wall -c a.c

b.o: b.c b.h
	gcc -Wall -c b.c

c.o: c.c c.h
	gcc -Wall -c c.c

clean:
	rm -f *.o kadai-a
