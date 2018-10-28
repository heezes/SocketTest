/*******************************************************************************
 * Vecmocon Technologies.
 * @author: Altamash Abdul Rahim
 *******************************************************************************/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "main.h"
#include "iot_platform.h"
#include "string.h"
#include "stdlib.h"


#pragma comment(lib,"ws2_32.lib")


char TimerIsExpired(Timer* timer) {
	long left = timer->end_time - GetTickCount();
	return (left < 0);
}


void TimerCountdownMS(Timer* timer, unsigned int timeout) {
	timer->end_time = GetTickCount() + timeout;
}


void TimerCountdown(Timer* timer, unsigned int timeout) {
	timer->end_time = GetTickCount() + (timeout * 1000);
}


int TimerLeftMS(Timer* timer) {
	long left = timer->end_time - GetTickCount();
	return (left < 0) ? 0 : left;
}


void TimerInit(Timer* timer) {
	timer->end_time = 0;
}


int read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	int recv_size = 0;
	setsockopt(n->my_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout_ms,sizeof(timeout_ms));
	recv_size = recv(n->my_socket, buffer, len, 0);
	if (recv_size == SOCKET_ERROR || recv_size == 0)
	{
		int ret = WSAGetLastError();
		if(ret!=10060)
			printf("\nReceive Error: %d\n", ret);
		return 0;
	}
	return recv_size;
}


int write(Network* n, unsigned char* buffer, int len, int timeout_ms) 
{
	if (send(n->my_socket,buffer, len, 0) < 0)
	{
		printf("\nSend Error: %d\n", WSAGetLastError());
		return -1;
	}
	return len;
}


void disconnect(Network* n) {
	n->my_socket = 0;
	n->mqttread = NULL;
	n->mqttwrite = NULL;
	closesocket(n->my_socket);
}


void NewNetwork(Network* n) {
	n->my_socket = 0;
	n->mqttread = read;
	n->mqttwrite = write;
	n->disconnect = disconnect;
}

int NetworkConnect(Network* n, char* endpoint, int port)
{
	WSADATA wsa;
	struct sockaddr_in server;
	char ip[100];
	struct hostent *he;
	struct in_addr **addr_list;
	int i;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return -1;
	}
	printf("Initialised.\n");
	if ((he = gethostbyname(endpoint)) == NULL)
	{
		//gethostbyname failed
		printf("gethostbyname failed : %d", WSAGetLastError());
		return -1;
	}
	//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
	addr_list = (struct in_addr **) he->h_addr_list;
	for (i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(ip, inet_ntoa(*addr_list[i]));
	}
	printf("%s resolved to : %s\n", endpoint, ip);
	//Create a socket
	if ((n->my_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		return -1;
	}
	printf("Socket created.\n");
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	//Connect to remote server
	if (connect(n->my_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("\nConnect Error: %d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}
