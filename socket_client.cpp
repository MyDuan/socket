// mysocketclient.cpp : �������̨Ӧ�ó������ڵ㡣
//
#pragma 


#include "stdafx.h"
#include "Winsock2.h"
//TCPͨ�ſͻ���
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

	int index = 0;
	SOCKET sockClient;
	SOCKADDR_IN addrClt;//��Ҫ���������IP��Ϣ 
	addrClt.sin_addr.S_un.S_addr=inet_addr("192.168.100.24");// inet_addr��IP��ַ�ӵ�����ʽת���������ֽڸ�ʽ���͡�  
	addrClt.sin_family=AF_INET;   
	addrClt.sin_port=htons(2000);
	char recvBuf[255];  
	char sendBuf[255];
	int err_1;
	sockClient=socket(AF_INET,SOCK_STREAM,0);
	err_1 = connect(sockClient,(SOCKADDR*)&addrClt,sizeof(SOCKADDR));

	while(1)
	{
		if (err_1 != 0)
		{
			err_1 = connect(sockClient,(SOCKADDR*)&addrClt,sizeof(SOCKADDR));
			index = 0;
		}
		memset(recvBuf, 0, sizeof(char)*255);
		memset(sendBuf, 0, sizeof(char)*255);
		recv(sockClient,recvBuf,255,0);  
		printf("my reply is : %s\n",recvBuf);  

		sprintf_s(sendBuf,"%3d,",index);  
		strcat_s(sendBuf,"haha");  
		send(sockClient,sendBuf,strlen(sendBuf)+1,0);

		index++;
		Sleep(200);
	}  

	closesocket(sockClient);
	WSACleanup();  
	return 0;
}

