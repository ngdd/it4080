
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
	int numRegisteredClients = 0;

	char buf[1024], cmdbuf[1024], filebuf[1024];
	char user[64], pass[64], tmp[64];

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
						
						// Xu ly buffer
						buf[res] = 0;
						res = sscanf(buf, "%s %s %s", user, pass, tmp);
						if (res != 2)
						{
							char * msg = "Wrong format. Please send again.\n";
							send(clients[i], msg, strlen(msg), 0);
						}
						else
						{
							sprintf(buf, "%s %s", user, pass);
							int found = 0;
							FILE *f = fopen("D:\\Test\\data.txt", "r");
							while (fgets(filebuf, sizeof(filebuf), f) != NULL)
							{
								filebuf[strlen(filebuf) - 1] = 0;
								if (strcmp(buf, filebuf) == 0)
								{
									found = 1;
									break;
								}
							}
							fclose(f);

							if (found == 0)
							{
								char * msg = "Wrong username or password. Please send again.\n";
								send(clients[i], msg, strlen(msg), 0);
							}
							else
							{
								char * msg = "OK. You can send command now.\n";
								send(clients[i], msg, strlen(msg), 0);

								registeredClients[numRegisteredClients] = clients[i];
								numRegisteredClients++;
							}
						}
					}
					else
					{
						// da dang nhap
						buf[res - 1] = 0;
						sprintf(cmdbuf, "%s > D:\\Test\\out.txt", buf);
						system(cmdbuf);

						FILE *f = fopen("D:\\Test\\out.txt", "r");
						while (fgets(filebuf, sizeof(filebuf), f) != NULL)
							send(clients[i], filebuf, strlen(filebuf), 0);
						fclose(f);
					}
				}
		}
	}

	return 0;
}