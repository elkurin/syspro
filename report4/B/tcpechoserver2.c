#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define SIZE 1024
#define max_clients 30

int main(int argc, char* argv[])
{
	if (argc != 2) {
		fprintf(stderr, "No port. %s\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	unsigned int clientAddrLength;
	unsigned short serverPort;
	char echoBuffer[SIZE + 1];
	int sendMessageSize;
	int receivedMessageSize;
	int client_socket[max_clients];
	int sd;

	int i;
	for (i = 0; i <  max_clients; i++) client_socket[i] = 0;

	serverPort = atoi(argv[1]);

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

	if (listen(sock, 10) < 0) {
		perror("listen() failed\n");
		exit(EXIT_FAILURE);
	}
	fd_set pfd;
	int fdmax = sock;
	printf("sock %d\n", sock);
	struct timeval tv;
	tv.tv_usec = 10000;

	while(1) {
		printf("hello\n");
		FD_ZERO(&pfd);
		FD_SET(sock, &pfd);
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];
			if (sd > 0) FD_SET(sd, &pfd);
			if (sd > fdmax) fdmax = sd;
		}
		if (select(fdmax + 1, &pfd, NULL, NULL, &tv) < 0) {
			perror("select() failed\n");
			exit(EXIT_FAILURE);
		}
		printf("selected\n");
		
		if (FD_ISSET(sock, &pfd)) {
			clientAddrLength = sizeof(clientAddr);
			int s = accept(sock, (struct sockaddr *)&clientAddr, &clientAddrLength);
			if (s < 0) {
				perror("accept() failed\n");
				exit(EXIT_FAILURE);
			}
			printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(clientAddr.sin_addr), sock);
			if (write(s, "Welcome!\n", 9) < 0) {
				perror("write() faileld\n");
				exit(EXIT_FAILURE);
			}
			for (i = 0; i < max_clients; i++) {
				if (!client_socket[i]) {
					client_socket[i] = s;
					break;
				}
			}
		}
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];
			if (FD_ISSET(sd, &pfd)) {
				memset(echoBuffer, '\0', sizeof(echoBuffer));
				receivedMessageSize = read(sd, echoBuffer, SIZE);
				if (receivedMessageSize < 0) {
					perror("read() failed\n");
					exit(EXIT_FAILURE);
				}
				sendMessageSize = write(sd, echoBuffer, SIZE);
				if (sendMessageSize < 0) {
					perror("write() failed\n");
					exit(EXIT_FAILURE);
				}
				printf("%s\n", echoBuffer);
				close(sd);
				client_socket[i] = 0;
			}
		}
	}

	return 0;
}
