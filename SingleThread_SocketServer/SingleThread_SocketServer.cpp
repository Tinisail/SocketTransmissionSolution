// SingleThread_SocketServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define SOCK_DEF_PORT 6011 //default port
#define SOCK_DEF_BUFFSIZE 1024 // default buffer size

int main(int argc, char * argv[])
{
	WSADATA wsa_data;
	SOCKET bind_soc = 0, acpt_soc = 0;

	struct sockaddr_in serv_addr, clnt_addr;
	unsigned short port = SOCK_DEF_PORT;
	char server_address[20] = { '\0' };

	int result, send_lenth = 0;
	int addr_len = sizeof(struct sockaddr_in);
	char send_buff[SOCK_DEF_BUFFSIZE] = {'\0'};
	char recv_buff[SOCK_DEF_BUFFSIZE] = {'\0'};

	if (argc < 2)
	{
		printf("\nPlease input server address [address]:");
		scanf("%s", server_address);

		printf("\nPlease input server listen port [port]:");
		scanf("%d", &port);
	}
	if (argc == 2)
	{
		strcpy(server_address, argv[1]);

		printf("\nPlease input server listen port [port]:");
		scanf("%d", &port);
	}
	if (argc == 3)
	{
		strcpy(server_address, argv[1]);
		port = atoi(argv[2]);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(server_address);

	// WinSock initial
	WSAStartup(MAKEWORD(2, 0), &wsa_data); 
	// create socket
	bind_soc = socket(AF_INET, SOCK_STREAM, 0);

	// bind socket
	result = bind(bind_soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (result == SOCKET_ERROR)
	{
		printf("Socket server: bind error : %d\n", WSAGetLastError());
		return -1;
	}

	//listen port
	listen(bind_soc, SOMAXCONN);

	printf("\nSocket Server is running ......\n");

	while (1)
	{
		printf("     Waiting for client requests.....\n");

		// accept sccket
		acpt_soc = accept(bind_soc, (struct sockaddr *)&clnt_addr, &addr_len);

		if (acpt_soc == INVALID_SOCKET)
		{
			printf("\nSocket Server: accept client socket connect error : %d\n", WSAGetLastError());
			break;
		}

		printf("\nSocket Server: accept client socket connect success, client socket addr : [IP:address] is [%s:%d]\n\n", 
			inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

		while (1)
		{
			result = recv(acpt_soc, recv_buff, SOCK_DEF_BUFFSIZE, 0);
			if (strcmp(recv_buff, "EXIT") == 0)
			{
				printf("Socket Server : Client socket process thread Exit.........\n");
				break;
			}
			if (result > 0)
			{
				printf("\nSocket Server : receive: [%s] from client socket [%s:%d]\n", recv_buff, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port) );
				memset(recv_buff, '\0', strlen(recv_buff));
				sprintf(send_buff, "I am the server, server addr : [%s:%d], client addr : [%s:%d]", 
					inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port) ,
					inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port) );
				result = send(acpt_soc, send_buff, strlen(send_buff), 0);
				printf("Socket Server : resend :%s\n", send_buff);
			}
		}

		closesocket(acpt_soc);

	}

	closesocket(bind_soc);
	WSACleanup();

	return 0;
}
