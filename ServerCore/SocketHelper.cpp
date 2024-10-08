#include "pch.h"
#include "SocketHelper.h"			

LPFN_ACCEPTEX SocketHelper::AcceptEx = nullptr;

bool SocketHelper::StartUp()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return false;

	//Accept �񵿱� �Լ� �ּҸ� ��� ���ؼ� �ӽ� ������ ����
	SOCKET tempSocket = CreateSocket();
	
	//AcceptEx�� Accept �񵿱� �Լ� �ּ� �־���
	SetIoControl(tempSocket, WSAID_ACCEPTEX, (LPVOID*)&AcceptEx);
		
	//�ӽ������� ����Ŵϱ� �ݾ������
	CloseSocket(tempSocket);

	return true;
}

bool SocketHelper::CleanUp()
{
	return WSACleanup();
}

bool SocketHelper::SetIoControl(SOCKET socket, GUID guid, LPVOID* func)
{
	DWORD dwBytes = 0;
	return WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), func, sizeof(*func), &dwBytes, NULL, NULL) != SOCKET_ERROR;

}

SOCKET SocketHelper::CreateSocket()
{
	return WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

void SocketHelper::CloseSocket(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
	{
		closesocket(socket);
		socket = INVALID_SOCKET;
	}
}

bool SocketHelper::SetReuseAddress(SOCKET socket, bool enable)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_REUSEADDR, enable);
}

bool SocketHelper::SetLinger(SOCKET socket, u_short onOff, u_short time)
{
	LINGER linger;
	linger.l_onoff = onOff;
	linger.l_linger = time;
	return SetSocketOpt(socket, SOL_SOCKET, SO_LINGER, linger);
}

bool SocketHelper::Bind(SOCKET socket, SOCKADDR_IN sockAddr)
{
	return bind(socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

bool SocketHelper::Listen(SOCKET socket, int baclog)
{
	return listen(socket, baclog) != SOCKET_ERROR;
}
