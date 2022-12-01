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
		//type이 DISCONNECT라면
	case IO_TYPE::DISCONNECT:
		//실행
		ProcessDisConnect();
		break;
	default:
		break;
	}
}

void Session::ProcessConnect()
{
	connectEvent.owner = nullptr;

	connected.store(true);

	GetService()->AddSession(GetSession());

	OnConnected();

	RegisterRecv();
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

	OnSend(bytes);
}
void Session::ProcessRecv(int32 bytes)
{
	recvEvent.owner = nullptr;

	if (bytes == 0)
	{
		Disconnect(L"Recv Data 0");
		return;
	}

	OnRecv(recvBuffer, bytes);

	RegisterRecv();
}

void Session::ProcessDisConnect()
{
	//그냥 밀어 버림
	disconnectEvent.owner = nullptr;
}

bool Session::Connect()
{
	return RegisterConnect();
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

bool Session::RegisterConnect()
{					 
	if (IsConnected())
	{
		return false;
	}

	if (GetService()->GetType() != SERVICE_TYPE::CLIENT)
	{
		return false;
	}

	if (SocketHelper::SetReuseAddress(socket, true) == false)
	{
		return false;	
	}

	if (SocketHelper::BindAny(socket, 0) == false)
	{
		return false;
	}

	connectEvent.Init();
	connectEvent.owner = shared_from_this();

	DWORD bytes = 0;
	SOCKADDR_IN sockAddrIn = GetService()->GetNetworkAddress().GetSockAddrIn();

	if (false == (SocketHelper::ConnectEx(socket, reinterpret_cast<SOCKADDR*>(&sockAddrIn), sizeof(sockAddrIn), nullptr, 0, &bytes, &connectEvent)))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			connectEvent.owner = nullptr; 
			return false;
		}

	}

	return true;


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

bool Session::RegisterDisconnect()
{
	//이벤트 초기화
	disconnectEvent.Init();
	//내 Session 연결
	disconnectEvent.owner = shared_from_this();

	//연결끊기
	//TF_REUSE_SOCKET : 소켓을 재활용 하겟다
	if (false == (SocketHelper::DisconnectEx(socket, &disconnectEvent, TF_REUSE_SOCKET, NULL)))
	{

		//기다리는중이 아니라면
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			//진짜 에러 
			//밀어 버리고
			disconnectEvent.owner = nullptr;
			return false;

		}
	}

	//성공
	return true;
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

	//오버라이드 된거 호출
	OnDisconnected();

	//SocketHelper::Close(socket);
	
	GetService()->RemoveSession(GetSession());

	//호출
	RegisterDisconnect();
}
