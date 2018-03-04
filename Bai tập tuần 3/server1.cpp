#include "stdafx.h"

#include "winsock2.h"

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	fd_set fdread;
	int res;
	SOCKET clients[64];
	int numClients = 0;
	SOCKET registeredClients[64];
	char * ids[64];
	int numRegisteredClients = 0;

	char buf[1024], sendbuf[1024];
	char cmd[16], id[64], tmp[64];

	while (true)
	{
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		res = select(0, &fdread, NULL, NULL, NULL);
		if (res == SOCKET_ERROR)
			break;
		if (res > 0)
		{
			if (FD_ISSET(listener, &fdread))
			{
				SOCKET client = accept(listener, NULL, NULL);
				clients[numClients] = client;
				numClients++;
			}
			for (int i = 0; i < numClients; i++)
				if (FD_ISSET(clients[i], &fdread))
				{
					// Xu ly du lieu nhan duoc tu client
					res = recv(clients[i], buf, sizeof(buf), 0);
					if (res == SOCKET_ERROR || res == 0)
						continue;

					int j = 0;
					for (; j < numRegisteredClients; j++)
						if (registeredClients[j] == clients[i])
							break;
					if (j >= numRegisteredClients)
					{
						// chua dang nhap
						buf[res] = 0;
						res = sscanf(buf, "%s %s %s", cmd, id, tmp);
						if (res != 2)
						{
							char * msg = "Wrong format. Please send again.\n";
							send(clients[i], msg, strlen(msg), 0);
						}
						else
						{
							if (strcmp(cmd, "client_id:") != 0)
							{
								char * msg = "Wrong format. Please send again.\n";
								send(clients[i], msg, strlen(msg), 0);
							}
							else
							{
								// Correct format
								char * msg = "OK. You can send message now.\n";
								send(clients[i], msg, strlen(msg), 0);

								registeredClients[numRegisteredClients] = clients[i];
								ids[numRegisteredClients] = (char *)malloc(64);
								memcpy(ids[numRegisteredClients], id, strlen(id) + 1);
								numRegisteredClients++;

								break;
							}
						}
					}
					else
					{
						// da dang nhap
						// Xu ly buffer
						buf[res] = 0;

						sprintf(sendbuf, "%s: %s", ids[j], buf);

						for (int j = 0; j < numRegisteredClients; j++)
							if (registeredClients[j] != clients[i])
								send(registeredClients[j], sendbuf, strlen(sendbuf), 0);
					}
				}
		}
	}

    return 0;
}
