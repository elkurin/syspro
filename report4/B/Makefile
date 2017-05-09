main: tcpechoserver1.c tcpechoserver2.c tcpechoclient.c
	gcc -Wall -o tcpechoserver1 tcpechoserver1.c -lpthread
	gcc -Wall -o tcpechoserver2 tcpechoserver2.c
	gcc -Wall -o tcpechoclient tcpechoclient.c

clean:
	rm -f tcpechoserver1 tcpechoserver2 tcpechoclient
