#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h" 


HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(socket);
}

void Session::Observe(IocpEvent* iocpEvent, int32 bytes)
{
	switch (iocpEvent->GetType())
	{
	case IO_TYPE::CONNECT:
		ProcessConnect();
		break;
	case IO_TYPE::RECV:
		ProcessRecv(bytes);
		break;
	case IO_TYPE::SEND:
		ProcessSend(static_cast<SendEvent*>(iocpEvent), bytes);
		break;
	default:
		break;
	}
}


void Session::ProcessSend(SendEvent* sendEvent, int32 bytes)
{
	sendEvent->owner = nullptr;
	delete(sendEvent);

	if (bytes == 0)
	{
		Disconnect(L"Send data is zero\n");
		return;
	}

	//컨텐츠에 있는 Session을 상속받은 class의 OnSend를 호출.
	OnSend(bytes);

}

void Session::Send(BYTE* buffer, int32 len)
{
	SendEvent* sendEvent = new SendEvent();
	sendEvent->owner = shared_from_this();
	sendEvent->buffer.resize(len);
	memcpy(sendEvent->buffer.data(), buffer, len);

	lock_guard<mutex> guard(lock);

	RegisterSend(sendEvent);
}

void Session::RegisterSend(SendEvent* sendEvent)
{
	if (IsConnected() == false)
	{
		return;
	}

	WSABUF dataBuf;
	dataBuf.buf = (char*)sendEvent->buffer.data();
	dataBuf.len = (ULONG)sendEvent->buffer.size();

	DWORD sendBytes = 0;
	if (SOCKET_ERROR == WSASend(socket, &dataBuf, 1, OUT &sendBytes, 0, sendEvent, NULL))
	{
		int32 errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent->owner = nullptr;
			delete(sendEvent);
		}

	}
	

}

void Session::ProcessConnect()
{
	connected.store(true);

	GetService()->AddSession(GetSession());

	//컨텐츠에 있는 Session을 상속받은 class의 OnConnected를 호출.
	OnConnected();

	RegisterRecv();
}



void Session::RegisterRecv()
{
	if (IsConnected() == false)
	{
		return;
	}

	recvEvent.Init();

	recvEvent.owner = shared_from_this();

	WSABUF dataBuf;
	dataBuf.buf = reinterpret_cast<char*>(recvBuffer);
	dataBuf.len = sizeof(recvBuffer);

	DWORD recvBytes = 0;
	DWORD flags = 0;

	if (WSARecv(socket, &dataBuf, 1, &recvBytes, &flags, &recvEvent, nullptr) == SOCKET_ERROR)
	{
		int32 errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			recvEvent.owner = nullptr;
		}

	}

}


void Session::ProcessRecv(int32 bytes)
{
	recvEvent.owner = nullptr; 

	if (bytes == 0)
	{
		Disconnect(L"Recv Data 0");
		return;
	}

	//컨텐츠에 있는 Session을 상속받은 class의 OnRecv를 호출.
	OnRecv(recvBuffer, bytes);
	//printf("Recv Data Length : %d byte\n", bytes);

	RegisterRecv();
}


Session::Session()
{
	socket = SocketHelper::CreateSocket();
}

Session::~Session()
{
	SocketHelper::Close(socket);
}


void Session::HandleError(int32 error)
{
	switch (error)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		printf("Handle Error : %d\n", error);
		break;
	}
}

void Session::Disconnect(const WCHAR* cause)
{
	if (connected.exchange(false) == false)
	{
		return;
	}

	printf("Disconnect : %ws", cause);

	SocketHelper::Close(socket);
	GetService()->RemoveSession(GetSession());
}
