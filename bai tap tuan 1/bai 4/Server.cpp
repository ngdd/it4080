// TextServer.cpp : Defines the entry point for the console application.
//

#include "stdio.h"

#include "winsock2.h"
#define _CRT_SECURE_NO_WARNINGS
#define BUFF_SIZE 1024

int main(int argc, char* argv[])
{
	// Kiem tra tham so dau vao
	if (argc != 2) {
		printf("Input error\n Ex:'Server.exe 8000'\n");
		system("pause");
		return 0;
	}

	int port = atoi(argv[1]);

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
	char buf[BUFF_SIZE] = "Hello Client";
	char bufRecv[BUFF_SIZE];

	//gửi tin nhắn đến client
	res = send(client, buf, strlen(buf), 0);
	if (res == 0 || res == SOCKET_ERROR) {
		printf("Có lỗi xảy ra");
		return 0;
	}

	//nhận tin nhắn từ clien
	while (1)
	{
		res = recv(client, bufRecv, sizeof(bufRecv), 0);
		bufRecv[res] = '\0';
		if (res == 0 || res == SOCKET_ERROR)
			break;

		printf("Message Client: %s\n", bufRecv);
	}

	closesocket(client);
	closesocket(listener);

	system("PAUSE");

	WSACleanup();
	return 0;
}

