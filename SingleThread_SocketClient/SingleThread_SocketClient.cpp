// SingleThread_SocketClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define SOCK_DEF_PORT 6011 //default port
#define SOCK_BUF_SIZE 1024 //default buffer size

int main(int argc, char * argv[])
{
	WSADATA wsa_data;
	SOCKET local_soc = 0; // socket label
	struct sockaddr_in serv_addr; // server address
	unsigned short port = SOCK_DEF_PORT;

	int result;
	char address[20] = { '\0' };
	char send_buff[SOCK_BUF_SIZE] = {'\0'};
	char recv_buff[SOCK_BUF_SIZE] = {'\0'};

	if (argc < 2)
	{
		printf("\nPlease input server address [address]:");
		scanf("%s", address);

		printf("\nPlease input server listen port [port]:");
		scanf("%d", &port);
	}
	if (argc == 2)
	{
		strcpy(address, argv[1]);

		printf("\nPlease input server listen port [port]:");
		scanf("%d", &port);
	}
	if (argc == 3)
	{
		strcpy(address, argv[1]);
		port = atoi(argv[2]);
	}

	// server address 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(address);

	WSAStartup(MAKEWORD(2, 0), &wsa_data); // WinSock initial

	if (serv_addr.sin_addr.s_addr == INADDR_NONE)
	{
		printf( "invalid address\n");
		return -1;
	}

	// create socket 
	local_soc = socket(AF_INET, SOCK_STREAM, 0);
	result = connect(local_soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (result == 0) // connect success
	{
		printf("\nSocket Client: connect success..........\n\n");
		sprintf(send_buff, "Client : I am a client. "); // send data

		do 
		{
			if (strlen(send_buff) != 0)
			{
				if (strcmp(send_buff, "EOF") == 0)
				{
					printf("Socket Client : Exit.........\n");
					break;
				}

				result = send(local_soc, (const char *)send_buff, strlen(send_buff), 0);
				printf("Socket Client : Send   : [%s] \n", send_buff);
				memset(send_buff, '\0', strlen(send_buff));
				result = recv(local_soc, recv_buff, SOCK_BUF_SIZE, 0);
				if (result > 0)  // send success
				{
					printf("Socket Client : Receive: [%s]\n", recv_buff );
					memset(recv_buff, '\0', strlen(recv_buff));
					printf("\nIf you want to exit, please input Ctrl-Z; or please input next messagge: ");
				}
				else
				{
					printf("Socket Client : receive error: %d\n", WSAGetLastError());
				}
			}

		} while (gets(send_buff));
		result = send(local_soc,"EXIT", strlen("EXIT"), 0);
		//		printf("%d\n", result);
		if (result <= 0)
		{
			printf("Socket Client : receive error: %d\n", WSAGetLastError());
		}
		else
			printf("Socket Client : Exit.........\n");
	}

	closesocket(local_soc);
	WSACleanup();

	system("pause");

	return 0;
}


