#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <sys/type.s>

#define SIZE 1024
#define num 100

int main(int argc, char* args[])
{
	if (argc != 2) {
		fprintf(stderr, "No port. %s\n", args[0]);
		exit(EXIT_FAILURE);
	}

	int sock;
	struct sockaddr_in serverAddr;
	unsigned short serverPort;
	char echoBuffer[SIZE];

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

	listen(sock, 100);
	fd_set pfd[num];
	struct timeval tv;
	tv.tv_sec = 5:
	tv.tv_usec = 0;

	if (select(0, pfd, NULL, NULL, &tv) < 0) {
		perror("select() failed\n");
		exit(EXIT_FAILURE);
	}
	int i;
	for (i = 0; i < num; i++) {
		if (pfd[i].revent & POLLIN) {
			memset(echoBuffer, 0, sizeof(echoBuffer));
			receivedMessasgeSize = read(pfd[i].fd, echoBuffer, SIZE);
			if (receivedMessageSize < 0) {
				perror("read() failed\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	return 0;
}
