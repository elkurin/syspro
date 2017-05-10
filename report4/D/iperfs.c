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

#define SIZE 1024

double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

void *serverRead(void *arg)
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
		dataSize += (long long)receivedMessageSize;
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

void *serverWrite(void *arg)
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
	if (argc != 2) {
		fprintf(stderr, "No port. %s\n", args[0]);
		exit(EXIT_FAILURE);
	}

	int sock, s;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	unsigned int clientAddrLength;
	unsigned short serverPort;

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
	s = accept(sock, (struct sockaddr *)&clientAddr, &clientAddrLength);
	if (s < 0) {
		perror("accept() failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Client connected. %s\n", inet_ntoa(clientAddr.sin_addr));

	pthread_t threadRead, threadWrite;
	if (pthread_create(&threadRead, NULL, serverRead, &s) != 0) {
		perror("pthread_create() failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&threadWrite, NULL, serverWrite, &s) != 0) {
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
	close(s);
	close(sock);
	return 0;
}


