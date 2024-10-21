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
	printf("~Session\n");
	SocketHelper::CloseSocket(socket);
}


void Session::RegisterRecv()
{
	if (!IsConnected())
		return;
		
	recvEvent.Init();
	//this -> shared_from_this()
	recvEvent.owner = shared_from_this();

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

bool Session::Connect()
{
	return RegisterConnect();
}
bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;	

	if (GetService()->GetServiceType() != ServiceType::CLIENT)
		return false;	

	if (SocketHelper::BindAny(socket, 0) == false)
		return false;

	connectEvent.Init();
	//this -> shared_from_this()
	connectEvent.owner = shared_from_this();

	DWORD bytesTransferred = 0;
	SOCKADDR_IN sockAddr = GetService()->GetSockAddr();
	if (SocketHelper::ConnectEx(socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr), nullptr, 0, &bytesTransferred, &connectEvent) == FALSE)
	{
		int errorCode = WSAGetLastError();

		if (errorCode != ERROR_IO_PENDING)
		{
			HandleError(errorCode);
			connectEvent.owner = nullptr;
			return false;
		}

	}

	return true;
}

void Session::ProcessConnect()
{
	connectEvent.owner = nullptr;

	isConnected.store(true);

	//this -> static_pointer_cast<Session>(shared_from_this())
	GetService()->AddSession(static_pointer_cast<Session>(shared_from_this()));

	OnConnected();

	RegisterRecv();
}




void Session::Send(BYTE* buffer, int len)
{
	SendEvent* sendEvent = new SendEvent();
	//this -> shared_from_this()
	sendEvent->owner = shared_from_this();

	sendEvent->sendBuffer.resize(len);

	memcpy(sendEvent->sendBuffer.data(), buffer, len);

	unique_lock<shared_mutex> lock(rwLock);
	RegisterSend(sendEvent);
}


void Session::RegisterSend(SendEvent* sendEvent)
{
	if (!IsConnected())
		return;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->sendBuffer.data();
	wsaBuf.len = sendEvent->sendBuffer.size();

	DWORD sendLen = 0;
	DWORD flags = 0;

	if (WSASend(socket, &wsaBuf, 1, &sendLen, flags, sendEvent, nullptr) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent->owner = nullptr;
			delete sendEvent;
		}
	}
}



void Session::ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred)
{
	switch (iocpEvent->eventType)
	{
	case EventType::CONNECT:
		ProcessConnect();
		break;
	case EventType::RECV:
		ProcessRecv(byteTransferred);
		break;
	case EventType::SEND:
		ProcessSend((SendEvent*)iocpEvent, byteTransferred);
	case EventType::DISCONNECT:	 
		ProcessDisconnect();
		break;
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
	sendEvent->owner = nullptr;
	delete sendEvent;

	if (bytesTransferred == 0)
	{
		Disconnect(L"Send 0 bytes");

	}

	OnSend(bytesTransferred);
}

void Session::Disconnect(const WCHAR* cause)
{
	if (isConnected.exchange(false) == false)
		return;

	wprintf(L"Disconnect reason : %ls\n", cause);

	OnDisconnected();
	//this -> static_pointer_cast<Session>(shared_from_this())
	GetService()->RemoveSession(static_pointer_cast<Session>(shared_from_this()));

	RegisterDisconnect();
}

bool Session::RegisterDisconnect()
{
	disconnectEvent.Init();
	//this -> shared_from_this()
	disconnectEvent.owner = shared_from_this();

	if (SocketHelper::DisconnectEx(socket, &disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int errorCode = WSAGetLastError();

		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			disconnectEvent.owner = nullptr;

			return false;
		}
	}

	return true;
}




void Session::ProcessDisconnect()
{
	//disconnectEvent owner는 null로 밀고
	disconnectEvent.owner = nullptr;
}

void Session::HandleError(int errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		//에러일 경우에도 Disconnect
		Disconnect(L"Handle Error");
		break;
	default:
		printf("Error Code : %d\n", errorCode);
		break;
	}
}

