#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SIZE 1024

int main(int argc, char* args[])
{
	if (argc != 2) {
		fprintf(stderr, "No port. %s\n", args[0]);
		exit(EXIT_FAILURE);
	}

	int sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	unsigned int clientAddrLength;
	char echoBuffer[SIZE + 1];
	unsigned short serverPort;
	int receivedMessageSize;
	int sendMessageSize;

	serverPort = atoi(args[1]);

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("socket() failed\n");
		exit(EXIT_FAILURE);
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(serverPort);

	int bindResult = bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (bindResult < 0) {
		perror("bind() failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Waiting to connect...\n");

	if (listen(sock, 10) < 0) {
		perror("listen() failed\n");
		exit(EXIT_FAILURE);
	}
	clientAddrLength = sizeof(clientAddr);
	int s = accept(sock, (struct sockaddr *)&clientAddr, &clientAddrLength);
	if (s < 0) {
		perror("accept() failed\n");
		exit(EXIT_FAILURE);
	}
	if (write(s, "Welcome!\n", 9) < 0) {
		perror("write() faileld\n");
		exit(EXIT_FAILURE);
	}
	printf("Client connected. %s\n", inet_ntoa(clientAddr.sin_addr));

	while(1) {
		memset(echoBuffer, '\0', sizeof(echoBuffer));
		receivedMessageSize = read(s, echoBuffer, SIZE);
		if (receivedMessageSize < 0) {
			perror("write() failed\n");
			exit(EXIT_FAILURE);
		}
		//printf("%s\n", echoBuffer);
		int i, flag = 0;
		for (i = 0; i < SIZE; i++) if (echoBuffer[i] == EOF) flag = 1;
		if (flag) break;
		/*
		sendMessageSize = write(s, echoBuffer, SIZE);
		if (sendMessageSize < 0) {
			perror("write() failed\n");
			exit(EXIT_FAILURE);
		}
		*/
	}
	write(s, "Data received. Shutting down...\n", 32);
	close(s);
	close(sock);
	return 0;
}


