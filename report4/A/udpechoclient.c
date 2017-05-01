#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char* args[])
{
	if (argc != 3) {
		fprintf(stderr, "No host or port. %s\n", args[0]);
		exit(EXIT_FAILURE);
	}

	int sock;
	struct sockaddr_in addr;
	unsigned int addrLength;
	struct hostent *server;
	char sendBuffer[255];
	char echoBuffer[255];
	char *hostName;
	unsigned short serverPort;
	int receivedMessageSize;
	int sendMessageSize;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket() failed\n");
		exit(EXIT_FAILURE);
	}

	hostName = args[1];
	serverPort = atoi(args[2]);

	server = gethostbyname(hostName);
	if (server == NULL) {
		fprintf(stderr, "No such host. %s\n", hostName);
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		  (char *)&addr.sin_addr.s_addr, server->h_length);
	addr.sin_port = htons(serverPort);

	while(1) {
		memset(sendBuffer, 0, sizeof(sendBuffer));
		memset(echoBuffer, 0, sizeof(echoBuffer));
		addrLength = sizeof(addr);
		scanf("%s", sendBuffer);
		if (sendBuffer[0] == EOF) break;
		sendMessageSize = sendto(sock, &sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&addr, addrLength);
		if (sendMessageSize < 0) {
			perror("sendto() failed\n");
			exit(EXIT_FAILURE);
		}
		receivedMessageSize = recvfrom(sock, echoBuffer, sizeof(echoBuffer), 0, (struct sockaddr *)&addr, &addrLength);
		if (receivedMessageSize < 0) {
			perror("recvfrom() failed\n");
			exit(EXIT_FAILURE);
		}
		printf("%s\n", echoBuffer);
	}
	close(sock);
	return 0;
}
