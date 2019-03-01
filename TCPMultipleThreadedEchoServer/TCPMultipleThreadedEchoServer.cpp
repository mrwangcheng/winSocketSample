#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include<process.h>
HANDLE g_hEvent = NULL;
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
struct Data{
	SOCKET sd;
	char tArr[];
};
unsigned __stdcall SendThread(void *lpvoid)
{	
	DWORD dReturn=WaitForSingleObject(g_hEvent, INFINITE);
	DWORD tid = GetCurrentThreadId();
	Data *pdata = (Data*)lpvoid;
	char temp[250] = { 0 };
	int sendCount, currentPosition = 0,len=0;
	len = strlen(pdata->tArr);
	sendCount = send(pdata->sd, pdata->tArr, len, 0);
	if (sendCount != len)
	{
		MessageBox(NULL,NULL,L"ERROR",MB_OK);
	}
	SetEvent(g_hEvent);
	return 0;

}
DWORD WINAPI ThreadProc(
	__in  LPVOID lpParameter
	)
{
	SOCKET AcceptSocket=(SOCKET) lpParameter;
	DWORD tid = GetCurrentThreadId();

	//���ջ������Ĵ�С��50���ַ�
	char recvBuf[51] = {0};
	char temp[250] = { 0 };
	HANDLE hThread = NULL;
	while(1){
		int count =recv(AcceptSocket ,recvBuf,50,0);
		strncpy(temp, recvBuf, strlen(recvBuf));
		sprintf(temp, "server recvive thread tid =(%d) message=%s", tid,recvBuf);
		Data *pdata = (Data*)malloc(sizeof(SOCKET)+50);
		pdata->sd = AcceptSocket;
		strcpy(pdata->tArr, recvBuf);
		int len = strlen(temp);
		if (count == 0 || count == SOCKET_ERROR)break;//���Է��ر�
		int sendCount,currentPosition=0;
		hThread = (HANDLE)_beginthreadex(NULL, 0, SendThread, (LPVOID)pdata, 0, NULL);
		if (!hThread)
		{
			WaitForSingleObject(hThread,INFINITE);
		}
		printf("\t\t\t�������Կͻ���%d����Ϣ��%s\n",AcceptSocket,recvBuf);
		Sleep(100);
	}
	//��������
	closesocket(AcceptSocket);
	return 0;
}

int main(int argc, char* argv[])
{	
	//----------------------
	// Initialize Winsock.
	g_hEvent = CreateEvent(NULL,FALSE,TRUE,NULL);
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", iResult);
		return 1;
	}
	//----------------------
	// Create a SOCKET for listening for
	// incoming connection requests.
	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY); //ʵ������0
	addrServer.sin_port = htons(20131);


	//���׽��ֵ�һ��IP��ַ��һ���˿���
	if (bind(ListenSocket,(SOCKADDR *) & addrServer, sizeof (addrServer)) == SOCKET_ERROR) {
		wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//���׽�������Ϊ����ģʽ�ȴ���������
	//----------------------
	// Listen for incoming connection requests.
	// on the created socket
	if (listen(ListenSocket, 5) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);
	//��һ������ѭ���ķ�ʽ����ͣ�ؽ��տͻ���socket����
	while(1)
	{
		//�������󣬽����������󣬷���һ���µĶ�Ӧ�ڴ˴����ӵ��׽���
		SOCKET AcceptSocket=accept(ListenSocket,(SOCKADDR*)&addrClient,&len);
		if(AcceptSocket  == INVALID_SOCKET)break; //����

		//�����߳�
		DWORD dwThread;
		HANDLE hThread = CreateThread(NULL,0,ThreadProc,(LPVOID)AcceptSocket,0,&dwThread);
		if(hThread==NULL)
		{
			closesocket(AcceptSocket);
			wprintf(L"Thread Creat Failed!\n");
			break;
		}

		CloseHandle(hThread);
	}

	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}

