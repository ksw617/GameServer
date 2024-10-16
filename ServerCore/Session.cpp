#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h"

Session::Session()
{
	socket = SocketHelper::CreateSocket();	
}

Session::~Session()
{
	SocketHelper::CloseSocket(socket);
}


void Session::ProcessConnect()
{
	isConnected.store(true);

	GetService()->AddSession(this);

	OnConnected();

	RegisterRecv();
}

void Session::RegisterRecv()
{
	if (!IsConnected())
		return;
		
	recvEvent.Init();
	recvEvent.owner = this;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)recvBuffer;
	wsaBuf.len = sizeof(recvBuffer);

	DWORD recvLen = 0;
	DWORD flags = 0;

	if (WSARecv(socket, &wsaBuf, 1, &recvLen, &flags, &recvEvent, nullptr) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			recvEvent.owner = nullptr;
		}
	}
	
}

void Session::Send(BYTE* buffer, int len)
{
	//SendEvent 만들기
	SendEvent* sendEvent = new SendEvent();

	//sendEvent의 주인은 나 자신
	sendEvent->owner = this;

	//sendEvent의 vector<Byte>의 크기값 설정
	sendEvent->sendBuffer.resize(len);

	//공간에다가 buffer의 내용을 복붙
	memcpy(sendEvent->sendBuffer.data(), buffer, len);

	//lock을 잡고
	unique_lock<shared_mutex> lock(rwLock);

	//보낼꺼 등록하기
	RegisterSend(sendEvent);
}


void Session::RegisterSend(SendEvent* sendEvent)
{
	//연결 상태가 아니라면 못 보냄
	if (!IsConnected())
		return;

	//WSASend 설정
	WSABUF wsaBuf;
	//sendEvent에 있는 버퍼 데이터와 크기
	wsaBuf.buf = (char*)sendEvent->sendBuffer.data();
	wsaBuf.len = sendEvent->sendBuffer.size();

	DWORD sendLen = 0;
	DWORD flags = 0;

	//WSASend 호출
	if (WSASend(socket, &wsaBuf, 1, &sendLen, flags, sendEvent, nullptr) == SOCKET_ERROR)
	{
		//에러 코드 받기
		int errorCode = WSAGetLastError();
		//에러 발생시
		if (errorCode != WSA_IO_PENDING)
		{
			//에러 표시
			HandleError(errorCode);
			//sendEvent의 주인은 없고
			sendEvent->owner = nullptr;

			//sendEvent 삭제
			delete sendEvent;
		}
	}
}


void Session::Disconnect(const WCHAR* cause)
{
	if (isConnected.exchange(false) == false)
		return;

	wprintf(L"Disconnect reason : $ls\n", cause);

	OnDisconnected();

	SocketHelper::CloseSocket(socket);

	GetService()->RemoveSession(this);
}

void Session::ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred)
{
	switch (iocpEvent->eventType)
	{
	case EventType::RECV:
		ProcessRecv(byteTransferred);
		break;
	case EventType::SEND:
		ProcessSend((SendEvent*)iocpEvent, byteTransferred);
	default:
		break;
	}

}

void Session::ProcessRecv(int bytesTransferred)
{

	recvEvent.owner = nullptr;

	if (bytesTransferred <= 0)
	{
		Disconnect(L"Recv 0 bytes");
		return;
	}

	OnRecv(recvBuffer, bytesTransferred);

	RegisterRecv();
}

void Session::ProcessSend(SendEvent* sendEvent, int bytesTransferred)
{
	printf("보냄\n");
}

void Session::HandleError(int errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		printf("Handle Error\n");
		break;
	default:
		break;
	}
}

