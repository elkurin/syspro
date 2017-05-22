main: parser/parse.c parser/print.c job.c
	gcc -c parser/parse.c
	gcc -c parser/print.c
	gcc -c job.c
	gcc -o job job.o parse.o print.o

clean:
	rm -f job.o parse.o print.o job
