#include "pch.h"
#include "SocketHelper.h"

void SocketHelper::Init()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAData wsaData;

	//CONDITION_CRASH(false);
	CONDITION_CRASH(WSAStartup(wVersionRequested,OUT &wsaData) == 0);
}

void SocketHelper::Clear()
{
	WSACleanup();
}

SOCKET SocketHelper::CreateSocket()
{
	return WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketHelper::SocketMode(SOCKET socket, GUID guid, LPVOID* lpfn)
{
	DWORD bytes = 0;
	//[in]  소켓
	//[in]  수행할 제어 작업 코드
	//[in]  입력 버퍼 연결 포인터
	//[in]  입력 버퍼의 크기(byte)
	//[out] 출력 버퍼 연결 포인터
	//[in]  출력 버퍼의 크기(byte)
	//[out] 출력의 식제 바이트 수에 대한 포인터
	//[in]  WSAOVERLAPPED 구조체에 대한 포인터(겹치지 않는 소켓의 경우 무시)
	//[in]  작업이 완료될때 호출되는 완료 루틴에 대한 포인터
	bool result = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), lpfn, sizeof(*lpfn), &bytes, NULL, NULL);
	return result != SOCKET_ERROR;
}
