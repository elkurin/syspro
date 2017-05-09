#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>

#define SIZE 1024

double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

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
	char sendBuffer[SIZE + 1];
	char echoBuffer[SIZE + 1];
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
	memset(echoBuffer, 0, sizeof(echoBuffer));
	if (read(sock, echoBuffer, SIZE) <  0) {
		perror("write() failed\n");
		exit(EXIT_FAILURE);
	}
	printf("%s\n", echoBuffer);
	/*
	int read_fd = open("foo.txt", O_RDONLY);
	if (read_fd < 0) {
		perror("open() failed\n");
		exit(EXIT_FAILURE);
	}
	*/

	double tstart = gettime();
	long long dataSize = 0;
	while(1) {
		memset(sendBuffer, '\0', sizeof(sendBuffer));
		memset(echoBuffer, '\0', sizeof(echoBuffer));
		/*
		ssize_t get = read(read_fd, sendBuffer, SIZE);
		if (get < 0) {
			perror("read() failed\n");
			exit(EXIT_FAILURE);
		} else if (!get) {
			sendBuffer[0] = EOF;
			if (write(sock, sendBuffer, SIZE) < 0)  {
				perror("write() failed\n");
				exit(EXIT_FAILURE);
			}
			break;
		}
		*/
		int i;
		for (i = 0; i < SIZE; i++) sendBuffer[i] = 'a' + i % 26;
		dataSize += (long long)SIZE;
		sendMessageSize = write(sock, sendBuffer, SIZE);
		if (sendMessageSize < 0) {
			perror("write() failed\n");
			exit(EXIT_FAILURE);
		}
		if (dataSize > 1e+10) {
			sendBuffer[0] = EOF;
			if (write(sock, sendBuffer, SIZE) < 0) {
				perror("write() failed\n");
				exit(EXIT_FAILURE);
			}
			break;
		}
		/*
		receivedMessageSize = read(sock, echoBuffer, SIZE);
		if (receivedMessageSize <  0) {
			perror("read() failed\n");
			exit(EXIT_FAILURE);
		}
		*/
		//printf("%s\n", sendBuffer);
	}
	printf("Sending finished.\n");
	memset(echoBuffer, '\0', sizeof(echoBuffer));
	if (read(sock, echoBuffer, SIZE) <  0) {
		perror("write() failed\n");
		exit(EXIT_FAILURE);
	}
	printf("%s\n", echoBuffer);
	double tend = gettime();
	printf("%ld %f\n", dataSize, tend - tstart);
	close(sock);
	return 0;
}
