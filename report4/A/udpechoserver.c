#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

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
	char echoBuffer[255];
	unsigned short serverPort;
	int receivedMessageSize;
	int sendMessageSize;

	serverPort = atoi(args[1]);

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
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

	while(1) {
		memset(echoBuffer, 0, sizeof(echoBuffer));
		clientAddrLength = sizeof(clientAddr);
		receivedMessageSize = recvfrom(sock, echoBuffer, sizeof(echoBuffer), 0, (struct sockaddr *)&clientAddr, &clientAddrLength);
		if (receivedMessageSize < 0) {
			perror("recvform() failed\n");
			exit(EXIT_FAILURE);
		}
		printf("Client connected. %s\n", inet_ntoa(clientAddr.sin_addr));
		printf("%s\n", echoBuffer);
		sendMessageSize = sendto(sock, &echoBuffer, sizeof(echoBuffer), 0, (struct sockaddr *)&clientAddr, clientAddrLength);
		if (sendMessageSize < 0) {
			perror("sendto() failed\n");
			exit(EXIT_FAILURE);
		}
	}
	close(sock);
	return 0;
}


