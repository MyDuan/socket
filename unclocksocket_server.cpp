#include "stdafx.h"

#include <iostream>   
#include <cassert>   
#include <list>   
#include <WinSock2.h>      
#define ASSERT assert 
using namespace std;
      
static const int c_iPort = 10001;   
bool GraceClose(SOCKET *ps); 

int _tmain(int argc, _TCHAR* argv[])
{
	int iRet;

	// initial Winsocket   
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
	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in adrServ;  
	ZeroMemory(&adrServ, sizeof(sockaddr_in));   
	adrServ.sin_family      = AF_INET;           
	adrServ.sin_port        = htons(c_iPort);     
	adrServ.sin_addr.s_addr = inet_addr("0.0.0.0");   
	bind(sockServer, (sockaddr*)&adrServ, sizeof(sockaddr_in));    
	listen(sockServer, FD_SETSIZE);    
	unsigned long ulMode = 1;   
	ioctlsocket(sockServer, FIONBIO, &ulMode);   

	fd_set fsListen;
	FD_ZERO(&fsListen);
	fd_set fsRead;   
	FD_ZERO(&fsRead); 

	timeval tv;   
	tv.tv_sec  = 1;   
	tv.tv_usec = 0; 

	list<SOCKET> sl;   
	int i = 2;
	while(1)
	{  
		// get the client socket  
		FD_SET(sockServer, &fsListen);
		iRet = select(1, &fsListen, NULL, NULL, &tv); 
		if(iRet > 0)   
		{   
			sockaddr_in adrClit;   
			int iLen = sizeof(sockaddr_in);   
			ZeroMemory(&adrClit, iLen);   
			SOCKET skAccept = accept(sockServer, (sockaddr*)&adrClit,&iLen);   
			ASSERT(INVALID_SOCKET != skAccept);   
			sl.push_back(skAccept); 
			cout << "New connection " << skAccept << ", c_ip: " 
				<< inet_ntoa(adrClit.sin_addr) << ", c_port: " << ntohs(adrClit.sin_port) << endl; 
		} 
  
		FD_ZERO(&fsRead);   
		for(list<SOCKET>::iterator iter = sl.begin(); iter != sl.end(); ++iter)   
		{   
			FD_SET(*iter, &fsRead);   
		} 

		iRet = select(sl.size(), &fsRead, NULL, NULL, &tv);   
		if(iRet > 0)   
		{   
			for(list<SOCKET>::iterator iter = sl.begin(); iter != sl.end(); ++iter)   
			{   
				iRet = FD_ISSET(*iter, &fsRead);
				if(iRet > 0)   
				{   
					const int c_iBufLen = 512;   
					char recvBuf[c_iBufLen + 1] = {'\0'};   
					int iRecv = SOCKET_ERROR;   
					iRecv = recv(*iter, recvBuf, c_iBufLen, 0);

					if (iRecv <= 0 )
					{   
					   iRecv == 0 ? cout << "Connection shutdown at socket " << *iter << endl :
							cout << "Connection recv error at socket " << *iter << endl;
						GraceClose(&(*iter));      
					}   
					else  
					{   
						recvBuf[iRecv] = '\0';   
						cout << "Server recved message from socket " << *iter << ": " << recvBuf << endl;   
						
						// set the write list 
						FD_SET fsWrite;   
						FD_ZERO(&fsWrite);   
						FD_SET(*iter, &fsWrite);   
						 
						iRet = select(1, NULL, &fsWrite, NULL, &tv);   
						if (iRet <= 0)   
						{   
							int iWrite = SOCKET_ERROR;   
							iWrite = send(*iter, recvBuf, iRecv, 0);   
							if (SOCKET_ERROR == iWrite)   
							{   
								cout << "Send message error at socket " << *iter << endl;   
								GraceClose(&(*iter));     
							}   
						}   
					}   
				}   
			}
			sl.remove(INVALID_SOCKET);
		}
	}

	ulMode = 0;   
	ioctlsocket(sockServer, FIONBIO, &ulMode);   
    GraceClose(&sockServer);   

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