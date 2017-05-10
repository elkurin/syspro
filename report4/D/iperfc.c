#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <netdb.h>

#define SIZE 1024

double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

void *clientRead(void *arg)
{
	int s = *(int *)arg;
	char echoBuffer[SIZE + 1];
	int receivedMessageSize;
	long long dataSize = 0;

	//計測開始
	double tstart = gettime();
	while(1) {
		memset(echoBuffer, '\0', sizeof(echoBuffer));
		receivedMessageSize = read(s, echoBuffer, SIZE);
		if (receivedMessageSize < 0) {
			perror("write() failed\n");
			exit(EXIT_FAILURE);
		}
		dataSize += receivedMessageSize;
		//データの分け方がclientとserverで違うことを確認する用
		//スループット計測時には時間のロスなのでカット
		//printf("GetSize: %d\n", receivedMessageSize);

		//EOFを受け取ったら終了する
		//ここですべての文字を確認しているので
		/*
		int i, flag = 0;
		for (i = 0; i < SIZE; i++) if (echoBuffer[i] == EOF) flag = 1;
		if (flag) break;
		*/
		if (echoBuffer[0] == EOF) break;
	}
	double tend = gettime();
	//計測終了
	printf("DataSize: %lld Byte\nTime: %f s\nThroughPut: %f Mbps\n", dataSize, tend - tstart, dataSize * 8 * 1e-6 / (tend - tstart));
}	

void *clientWrite(void *arg)
{
	int s = *(int *)arg;
	char sendBuffer[SIZE + 1];
	int sendMessageSize;

	long long dataSize = 0;
	while(1) {
		memset(sendBuffer, '\0', sizeof(sendBuffer));
		int i;
		for (i = 0; i < SIZE; i++) sendBuffer[i] = 'a' + i % 26;
		dataSize += (long long)SIZE;
		sendMessageSize = write(s, sendBuffer, SIZE);
		if (sendMessageSize < 0) {
			perror("write() failed\n");
			exit(EXIT_FAILURE);
		}
		if (dataSize > 1e+7) {
			//1024文字のEOFを送ることでserver側も必ず1文字目にEOFが来るので1文字目の確認だけで終了を読み取れる
			//これで高速化した
			memset(sendBuffer, EOF, sizeof(sendBuffer));
			if (write(s, sendBuffer, SIZE) < 0) {
				perror("write() failed\n");
				exit(EXIT_FAILURE);
			}
			break;
		}
	}
	printf("Sending finished.\n");
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
	char *hostName;
	unsigned short serverPort;

	hostName = args[1];
	serverPort = atoi(args[2]);

	server = gethostbyname(hostName);
	if (server == NULL) {
		fprintf(stderr, "No such host. %s\n", hostName);
		exit(EXIT_FAILURE);
	}

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("socket() failed\n");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		  (char *)&addr.sin_addr.s_addr, server->h_length);
	addr.sin_port = htons(serverPort);

	addrLength = sizeof(addr);
	int connectResult = connect(sock, (struct sockaddr *)&addr, addrLength);
	if (connectResult < 0) {
		perror("connect() failed\n");
		exit(EXIT_FAILURE);
	}


	pthread_t threadRead, threadWrite;
	if (pthread_create(&threadRead, NULL, clientRead, &sock) != 0) {
		perror("pthread_create() failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&threadWrite, NULL, clientWrite, &sock) != 0) {
		perror("pthread_create() failed\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(threadRead, NULL) != 0) {
		perror("pthread_join() failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_join(threadWrite, NULL) != 0) {
		perror("pthread_join() failed\n");
		exit(EXIT_FAILURE);
	}
	close(sock);
	return 0;
}


