// client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>   
#include <cassert>   
#include <WinSock2.h>   
#pragma comment(lib, "ws2_32.lib")   
using namespace std;

#define ASSERT assert 

static const char c_szIP[] = "192.168.100.24";   
static const int  c_iPort  = 10001; 

bool GraceClose(SOCKET *ps);   
int main()   
{   
	int iRet;

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

	SOCKET skClient;   
	skClient = socket(AF_INET, SOCK_STREAM, 0);   

	sockaddr_in adrServ;                         
	ZeroMemory(&adrServ, sizeof(sockaddr_in));         
	adrServ.sin_family      = AF_INET;                
	adrServ.sin_port        = htons(c_iPort);         
	adrServ.sin_addr.s_addr = inet_addr(c_szIP); 
  
	unsigned long ulEnable = 1;   
	ioctlsocket(skClient, FIONBIO, &ulEnable);   

	fd_set fsWrite;   
	TIMEVAL tv;

	tv.tv_sec  = 1;   
	tv.tv_usec = 0;  

	cout << "Client began to connect to the server..." << endl;   
	while(1)  
	{   
		iRet = connect(skClient, (sockaddr*)&adrServ, sizeof(sockaddr_in));   
		int iErrorNo = SOCKET_ERROR;   
		int iLen = sizeof(int);   
		if (SOCKET_ERROR == iRet &&	0 != getsockopt(skClient, SOL_SOCKET, SO_ERROR, (char*)&iErrorNo, &iLen))   
		{   
			cout << "An error happened on connecting to server. The error no is " << iErrorNo   
				<< ". The program will exit now." << endl;   
			exit(-1);   
		}   

		FD_ZERO(&fsWrite);   
		FD_SET(skClient, &fsWrite);   
		iRet = select(1, NULL, &fsWrite, NULL, &tv);   
		if (iRet > 0)   
		{   
			cout << "Successed connect to the server..." << endl;   
			break;   
		}  
		cout << "retrying" << endl;
	}

	while(1)   
	{   
		const int c_iBufLen =512;   
		char szBuf[c_iBufLen + 1] = {'\0'};   
		cout << "What you will say:";   
		cin  >> szBuf;   
		if(0 == strcmp("exit", szBuf))   
		{   
			break;   
		}   

		FD_ZERO(&fsWrite);   
		FD_SET(skClient, &fsWrite);   

		iRet = select(1, NULL, &fsWrite, NULL, &tv);   
		if (0 < iRet)   
		{   
			iRet = send(skClient, szBuf, strlen(szBuf), 0);    
			if(SOCKET_ERROR == iRet)   
			{   
				cout << "send error." << endl;   
				break;   
			}   

			fd_set fsRead;   
			FD_ZERO(&fsRead);   
			FD_SET(skClient, &fsRead);  

			iRet = select(1, &fsRead, NULL, NULL, &tv);   
			if (0 < iRet)   
			{   
				iRet = recv(skClient, szBuf, c_iBufLen, 0);   
				if(0 == iRet)   
				{   
					cout << "connection shutdown." << endl;   
					break;   
				}   
				else if(SOCKET_ERROR == iRet)   
				{   
					cout << "recv error." << endl;   
					break;   
				}   
				szBuf[iRet] = '\0';   
				cout << szBuf << endl;   
			}   
		}
	}

	
	ioctlsocket(skClient, FIONBIO, &ulEnable);   
    GraceClose(&skClient);    
	WSACleanup();   
  
	system("pause");   
	return 0;   
}   
bool GraceClose(SOCKET *ps)   
{   
	const int c_iBufLen = 512;   
	char szBuf[c_iBufLen + 1] = {'\0'};   
	// 关闭该套接字的连接   
	int iRet = shutdown(*ps, SD_SEND);   
	while(recv(*ps, szBuf, c_iBufLen, 0) > 0);   
	if (SOCKET_ERROR == iRet)   
	{   
		return false;   
	}   
	// 清理该套接字的资源   
	iRet = closesocket(*ps);   
	if (SOCKET_ERROR == iRet)   
	{   
		return false;   
	}   
	*ps = INVALID_SOCKET;   
	return true;   
}  
