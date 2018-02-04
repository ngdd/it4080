// TextServer.cpp : Defines the entry point for the console application.
//

#include "stdio.h"

#include "winsock2.h"
#define _CRT_SECURE_NO_WARNINGS

int main(int argc, char* argv[])
{
	// Kiem tra tham so dau vao
	if (argc != 4) {
		printf("Input error\n Ex:'TCPClient.exe 8000 E:\\hello.txt E:\\savefile.txt'\n");
		system("pause");
		return 0;
	}

	int port = atoi(argv[1]);
	char *fpathHello = argv[2];
	char *fpathClient = argv[3];

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	SOCKET client = accept(listener, NULL, NULL);

	int res;
	float n = 0;
	char buf[1024];

	FILE *fClient = fopen(fpathClient, "r");
	FILE *fHello = fopen(fpathHello, "r");

	//gửi tin nhắn đến client
	while (fscanf(fHello, "%s", buf) != EOF);
	fclose(fHello);
	res = send(client, buf, strlen(buf), 0);
	if (res == 0 || res == SOCKET_ERROR) {
		printf("Có lỗi xảy ra");
		return 0;
	}

	//nhận tin nhắn từ clien
	while (1)
	{
		res = recv(client, buf, sizeof(buf), 0);
		buf[res] = '\0';
		if (res == 0 || res == SOCKET_ERROR)
			break;

		printf("Message Client: %s\n", buf);
		fwrite(buf, 1, res, fClient);
	}

	fflush(fClient);
	fclose(fClient);
	fflush(fHello);
	fclose(fHello);

	closesocket(client);
	closesocket(listener);

	system("PAUSE");

	WSACleanup();
	return 0;
}

