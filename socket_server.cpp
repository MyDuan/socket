// mysoket.cpp : 定义控制台应用程序的入口点。
//
#pragma 

#include "stdafx.h"
#include "Winsock2.h"
//TCP通信服务器端
int _tmain(int argc, _TCHAR* argv[])
{
	WORD versionRequest;
	versionRequest = MAKEWORD(1, 1);
	WSADATA wsaData;
	int err = WSAStartup(versionRequest, &wsaData);
	if(err != 0)
	{
		return 0;
	}
	if( LOBYTE(wsaData.wVersion)!= 1  || HIBYTE(wsaData.wVersion)!= 1)
	{
		WSACleanup();
		return 0;
	}
 
	SOCKET sockServer;
	sockServer = socket(AF_INET, SOCK_STREAM, 0);//在网络中通信，流套接字， 网络协议为TCP/IP
	SOCKADDR_IN address ;
	address.sin_family = AF_INET;
	address.sin_addr.S_un.S_addr=inet_addr("0.0.0.0");
	address.sin_port = htons(2000);//选1024以上的
	bind(sockServer, (SOCKADDR*)&address , sizeof(SOCKADDR));
	listen(sockServer, 5);
	
	SOCKET sockClient;
	SOCKADDR_IN addClient;
	char sendBuf[255];
	char recvBuf[255]; 
	int addLen = sizeof(SOCKADDR);
	sockClient = accept(sockServer, (SOCKADDR*)&addClient, &addLen);

	while(1)
	{
		memset(sendBuf, 0, sizeof(char)*255);
		memset(recvBuf, 0, sizeof(char)*255);
		
		sprintf_s(sendBuf,"Welcome %s to here!",inet_ntoa(addClient.sin_addr));
		send(sockClient, sendBuf, strlen(sendBuf)+1, 0);
		recv(sockClient,recvBuf, 255,0); 
		
		printf("%s\n", recvBuf);
		Sleep(200);
	}

	closesocket(sockClient);
	WSACleanup();
	return 0;
}

