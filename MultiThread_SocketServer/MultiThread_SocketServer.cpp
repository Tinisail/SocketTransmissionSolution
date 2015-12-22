// MultiThread_SocketServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <WinSock2.h>
#include <WINDOWS.H>

#define SOCK_DEF_PORT 6011 //default port
#define SOCK_DEF_BUFFSIZE 1024 // default buffer size
#define MAX_SOCKET_COUNTS 32  // max socket counts for client socket

typedef struct _SOCKETDATA 
{
	SOCKET acpt_soc;                // accept socket
	struct sockaddr_in clnt_addr;   // client addr
	struct sockaddr_in serv_addr;   // server addr
} SOCKETDATA, * PSOCKETDATA ;

static unsigned _stdcall ClientSocketProcessThread(void* lpParam);
static unsigned _stdcall ReceiveDataThread(void* lpParam);
static unsigned _stdcall SendDataThread(void* lpParam);

int main(int argc, char * argv[])
{
	WSADATA wsa_data;
	SOCKET bind_soc = 0, acpt_soc = 0;

	HANDLE hSocketClientProcess[MAX_SOCKET_COUNTS] = {0} ; // store socket client data process handle
	int handleCount = 0;

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

		if ( handleCount < MAX_SOCKET_COUNTS )  // don't reach the MAX_SOCKET_COUNTS:32
		{
			printf("\nSocket Server: accept client socket connect success, client socket addr : [IP:address] is [%s:%d]\n\n", 
				inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

			// prepare data for client-socket process thread
			SOCKETDATA acpt_sock_data;
			memset(&acpt_sock_data, 0, sizeof(SOCKETDATA));
			acpt_sock_data.acpt_soc = acpt_soc;
			memcpy(&acpt_sock_data.clnt_addr, &clnt_addr, sizeof(struct sockaddr_in));
			memcpy(&acpt_sock_data.serv_addr, &serv_addr, sizeof(struct sockaddr_in));

			// create client-socket process thread and transfer parameter
			hSocketClientProcess[handleCount] = (HANDLE)_beginthreadex(NULL,0,&ClientSocketProcessThread,(LPVOID*)&acpt_sock_data,0,NULL);
			handleCount++;
		} 
		else
		{
			printf("\nSocket Server: thread counts for client socket counts has reached the MAX_SOCKET_COUNTS:32\n\n");
		}

		Sleep(1); // activate reschedule
	}

	closesocket(bind_soc);
	WSACleanup();

	WaitForMultipleObjects(handleCount, hSocketClientProcess, TRUE, INFINITE);

	return 0;
}

unsigned _stdcall ClientSocketProcessThread(void* lpParam)
{
	SOCKETDATA socketData = {0};
	memcpy(&socketData, lpParam, sizeof(socketData));

	SOCKET acpt_soc = socketData.acpt_soc;
	struct sockaddr_in clnt_addr, serv_addr;
	memcpy(&clnt_addr, &(socketData.clnt_addr), sizeof(struct sockaddr_in));
	memcpy(&serv_addr, &(socketData.serv_addr), sizeof(struct sockaddr_in));

	char send_buff[SOCK_DEF_BUFFSIZE] = {'\0'};
	char recv_buff[SOCK_DEF_BUFFSIZE] = {'\0'};
	int result = -1;

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

		Sleep(1); // activate reschedule
	}

	closesocket(acpt_soc);

	return (unsigned int)0;
}

unsigned _stdcall ReceiveDataThread(void* lpParam)
{
	return (unsigned int)0;
}

unsigned _stdcall SendDataThread(void* lpParam)
{
	return (unsigned int)0;
}