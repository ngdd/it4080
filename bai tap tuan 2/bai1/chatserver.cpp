

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "process.h"



typedef struct client {
	SOCKET sockclient=0;
	char* id;
}client;

client session[30];
char* checkId(char* mess) {
	if (strstr(mess, "client_id: ") == mess) {
		return mess + sizeof("client_id: ");
	}
	else
		return NULL;
}

unsigned __stdcall echoThread(void *param) {
	int ret;
	char mess[1024];
	int position;
	SOCKET sockclient = (SOCKET)param;
	for (int i = 0; i < 30; i++) {
		if (session[i].sockclient == 0) {
			session[i].sockclient = sockclient;
			position = i;
			break;
		}

	}
	//receive message from client
	while (true) {
		ret = send(sockclient, "Please enter your id", strlen("Please enter your id"), 0);
		if (ret == SOCKET_ERROR) {
			printf("Error! Cannot send mesage.");
			return 0;
		}
		ret = recv(sockclient, mess, sizeof(mess), 0);
		if (ret == SOCKET_ERROR) {
			printf("\nClient disconnected");
			break;
		}
		else {
			mess[ret] = 0;
			session[position].id = checkId(mess);
			if (session[position].id == NULL) {
				ret = send(sockclient, "Wrong form!!!", strlen("Wrong form!!!"), 0);
				if (ret == SOCKET_ERROR) {
					printf("Error! Cannot send mesage.");
					return 0;
				}
				continue;
			}
			else {
				ret = send(sockclient, "OK", strlen("OK"), 0);
				if (ret == SOCKET_ERROR) {
					printf("Error! Cannot send mesage.");
					return 0;
				}
				break;
			}
		}
	}
	while (true) {
		ret = recv(sockclient, mess, sizeof(mess), 0);
		if (ret == SOCKET_ERROR) {
			printf("\nClient disconnected");
			break;
		}
		else {
			mess[ret] = 0;
			if (strcmp(mess, "bye") == 0)
				break;
			else{
				for (int i = 0; i < 30; i++) {
					if (session[i].sockclient > 0 && i != position) {
						char s[256];
						sprintf(s, "%s: %s", session[i].id, mess);
						ret = send(session[i].sockclient, s, strlen(s), 0);
						if (ret == SOCKET_ERROR) {
							printf("Error! Cannot send mesage.");
							return 0;
						}
					}
				}
			}
		}
	}
	session[position].sockclient = 0;
	closesocket(sockclient);
	return 0;
}

int main(int argc, CHAR* argv[])
{
	//Check argument
	if (argc != 2) {
		printf("Usage: %s <portnumber>\n", argv[0]);
		_getch();
		return 0;
	}
	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		printf("Version is not supported\n");

	//Step 2: Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error! Cannot bind this address.");
		_getch();
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error! Cannot listen.");
		_getch();
		return 0;
	}

	printf("Server started!");

	//Step 5: Communicate with client
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	SOCKET sockclient;
	while (1) {
		
		//accept request
		sockclient = accept(listenSock, (sockaddr *)& clientAddr, &clientAddrLen);
		_beginthreadex(0, 0, echoThread, (void *)sockclient, 0, 0); //start thread
		
	} //end accepting

	  //Step 5: Close socket
	closesocket(listenSock);

	//Step 6: Terminate Winsock
	WSACleanup();

	return 0;
}


