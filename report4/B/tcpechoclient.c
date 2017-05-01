#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SIZE 1024

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
	char sendBuffer[SIZE];
	char echoBuffer[SIZE];
	char *hostName;
	unsigned short serverPort;
	int receivedMessageSize;
	int sendMessageSize;

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
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock < 0) {
			perror("socket() failed\n");
			exit(EXIT_FAILURE);
		}
		addrLength = sizeof(addr);
		int connectResult = connect(sock, (struct sockaddr *)&addr, addrLength);
		if (connectResult < 0) {
			perror("connect() failed\n");
			exit(EXIT_FAILURE);
		}

		memset(sendBuffer, 0, sizeof(sendBuffer));
		memset(echoBuffer, 0, sizeof(echoBuffer));
		scanf("%s", sendBuffer);
		if (sendBuffer[0] == EOF) break;
		sendMessageSize = write(sock, sendBuffer, SIZE);
		if (sendMessageSize < 0) {
			perror("write() failed\n");
			exit(EXIT_FAILURE);
		}
		receivedMessageSize = read(sock, echoBuffer, SIZE);
		if (receivedMessageSize <  0) {
			perror("read() failed\n");
			exit(EXIT_FAILURE);
		}
		printf("%s\n", echoBuffer);
		close(sock);
	}
	return 0;
}
