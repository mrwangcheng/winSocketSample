#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <io.h>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
    //----------------------
    // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error: %ld\n", iResult);
        return 1;
    }

    //----------------------
    // Create a SOCKET for connecting to server
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        return 1;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound.
    sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    addrServer.sin_port = htons(20131);

	//----------------------
    // Connect to server.
    iResult = connect( ConnectSocket, (SOCKADDR*) &addrServer, sizeof(addrServer) );
    if ( iResult == SOCKET_ERROR) {
        closesocket (ConnectSocket);
        printf("Unable to connect to server: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

	//char buf[1024 + 1] = {0};
	//以一个无限循环的方式，不停地接收输入，发送到server
	FILE *fp=NULL,*fpw=NULL;
	fp = fopen("D:\\winSocketSample\\Debug\\test.txt","r");
	if (fp == NULL){
		DWORD nEr = GetLastError();
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	long size=ftell(fp);
	char *buf = new char[size + 1];
	memset(buf,0,strlen(buf)+1);
	fseek(fp,0,SEEK_SET);
	fread(buf, 1, size, fp);
	int sentsize = 1024;
	while(1)
	{	
		//int count = _read (0, buf, 1024);//从标准输入读入
		long sendCount=0,currentPosition=0;
	
			sendCount = send(ConnectSocket, buf + currentPosition, sentsize, 0);
			if (sendCount == size)
			{
				break;
			}
			else{
				currentPosition += sendCount;
			}
		//Sleep(1000);
		if(sendCount==SOCKET_ERROR)break;
		
		long count =recv(ConnectSocket ,buf,size,0);
		if(count==0)break;//被对方关闭
		if(count==SOCKET_ERROR)break;//错误count<0
		buf[count]='\0';
		printf("%s",buf);
	}
	//结束连接
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}

