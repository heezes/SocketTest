#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h> 
#include <stdint.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

void main()
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char *message, server_reply[2000];
	int recv_size;

	char *hostname = "iot.eclipse.org";
	char ip[100];
	struct hostent *he;
	struct in_addr **addr_list;
	int i;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		goto exit;
	}

	printf("Initialised.\n");


	if ((he = gethostbyname(hostname)) == NULL)
	{
		//gethostbyname failed
		printf("gethostbyname failed : %d", WSAGetLastError());
		goto exit;
	}

	//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
	addr_list = (struct in_addr **) he->h_addr_list;

	for (i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(ip, inet_ntoa(*addr_list[i]));
	}

	printf("%s resolved to : %s\n", hostname, ip);

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		goto exit;
	}
	printf("Socket created.\n");


	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(1883);

	//Connect to remote server
	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("\nConnect Error: %d\n", WSAGetLastError());
		goto exit;
	}
	puts("Connected");
	/*
	//Send some data
	message = "GET / HTTP/1.1\r\n\r\n";
	if (send(s, message, strlen(message), 0) < 0)
	{
		puts("Send failed");
		goto exit;
	}
	puts("Data Send\n");

	//Receive a reply from the server
	if ((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR)
	{
		puts("recv failed");
		goto exit;
	}

	puts("Reply received\n");
	printf("%s\n", server_reply);
	*/
	goto finish;
exit:
	puts("\r\n------------------------------------Sys Exit------------------------------------\r\n");
finish:
	puts("\r\n------------------------------------Task Done------------------------------------\r\n");
}