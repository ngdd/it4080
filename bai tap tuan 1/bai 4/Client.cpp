#include <iostream>
#include <winsock2.h>

#define BUFF_SIZE 1024
#define _WINSOCK_DEPRECATED_NO_WARNINGS

int main(int argc, char** argv) {

	// Kiem tra tham so dau vao
	if (argc != 3) {
		printf("Input error\n Ex:'TCPClient.exe 127.0.0.1 8000'\n");
		system("pause");
		return 0;
	}

	int portServer = atoi(argv[2]);
	char *ipServer = argv[1];

	WSADATA wsaData;

	// Khoi tao winsock 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		printf("Version is not supported\n");
		system("pause");
		return 0;
	}

	// Tao socket client
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Khoi tao cau truc SOCKADDR_IN cua server
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portServer);
	serverAddr.sin_addr.s_addr = inet_addr(ipServer);

	// Ket noi server thong qua socket newConnection
	if (connect(client, (SOCKADDR*)&serverAddr, sizeof(serverAddr))) {
		printf("Cannot connect server\n");
		system("pause");
		return 0;
	}

	// Bat dau gui nhan du lieu
	char buf[BUFF_SIZE];
	char bufRecv[BUFF_SIZE];
	int res;

	res = recv(client, bufRecv, sizeof(bufRecv), 0);
	bufRecv[res] = '\0';
	if (res == 0 || res == SOCKET_ERROR) {
		printf("Có lỗi xảy ra");
		return 0;
	}
	printf("Message Server: %s\n", bufRecv);

	while (1) {
		printf("Send to server: ");
		gets_s(buf, BUFF_SIZE);
		res = send(client, buf, strlen(buf), 0);
		if (strcmp(buf, "exit") == 0) break;
		if (res == SOCKET_ERROR) {
			printf("Error\n");
			break;
		}
		printf("Nhap 'exit' de ket thuc\n");
	}

	// Dong socket
	closesocket(client);

	// Giai phong winsock
	WSACleanup();

	system("pause");
	return 0;
}