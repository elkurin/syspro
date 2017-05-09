#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define thread_num 100
#define SIZE 1024

void *server(void *arg)
{
	int s, ss = *(int *)arg;
	struct sockaddr_in clientAddr;
	unsigned int clientAddrLength;
	char echoBuffer[SIZE + 1];
	int receivedMessageSize;
	int sendMessageSize;

	while(1) {
		clientAddrLength = sizeof(clientAddr);
		s = accept(ss, (struct sockaddr *)&clientAddr, &clientAddrLength);
		if (s < 0) {
			perror("accept() failed\n");
			exit(EXIT_FAILURE);
		}
		if (write(s, "Welcome!\n", 9) < 0) {
			perror("write() faileld\n");
			exit(EXIT_FAILURE);
		}
		memset(echoBuffer, '\0', sizeof(echoBuffer));
		receivedMessageSize = read(s, echoBuffer, SIZE);
		if (receivedMessageSize < 0) {
			perror("read() failed\n");
			exit(EXIT_FAILURE);
		}
		sendMessageSize = write(s, echoBuffer, SIZE);
		if (sendMessageSize < 0) {
			perror("write() failed\n");
			exit(EXIT_FAILURE);
		}
		printf("%s\n", echoBuffer);
		close(s);
	}
}

int main(int argc, char* args[])
{
	if (argc != 2) {
		fprintf(stderr, "No port. %s\n", args[0]);
		exit(EXIT_FAILURE);
	}

	int sock;
	struct sockaddr_in serverAddr;
	unsigned short serverPort;

	serverPort = atoi(args[1]);

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("socket() failed\n");
		exit(EXIT_FAILURE);
	}
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(serverPort);

	int bindResult = bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (bindResult < 0) {
		perror("bind() failed\n");
		exit(EXIT_FAILURE);
	}

	if (listen(sock, 100) < 0) {
		perror("listen() failed\n");
		exit(EXIT_FAILURE);
	}
	
	int i;
	pthread_t thread[thread_num];
	for (i = 0; i < thread_num; i++) {
		if (pthread_create(&thread[i], NULL, server, &sock) != 0) {
			perror("pthread_create() failed\n");
			exit(EXIT_FAILURE);
		}
	}
	for (i = 0; i < thread_num; i++) {
		if (pthread_join(thread[i], NULL) != 0) {
			perror("pthread_join() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
