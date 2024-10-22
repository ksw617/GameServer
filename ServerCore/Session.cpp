#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h"

Session::Session() : recvBuffer(BUFFER_SIZE) 
{
	socket = SocketHelper::CreateSocket();	
}

Session::~Session()
{
	SocketHelper::CloseSocket(socket);
}


void Session::RegisterRecv()
{
	if (!IsConnected())
		return;
		
	recvEvent.Init();
	recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;

	wsaBuf.buf = (char*)recvBuffer.WritePos();
	wsaBuf.len = recvBuffer.FreeSize();

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

void Session::ProcessRecv(int bytesTransferred)
{
	recvEvent.owner = nullptr;

	if (bytesTransferred <= 0)
	{
		Disconnect(L"Recv 0 bytes");
		return;
	}

	if (recvBuffer.OnWrite(bytesTransferred) == false)
	{
		Disconnect(L"On write overflow");
		return;
	}

	uint32 dataSize = recvBuffer.DataSize();

	uint32 processLen = OnRecv(recvBuffer.ReadPos(), bytesTransferred);
	if (processLen < 0 || dataSize < processLen || recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"On read overflow");
		return;
	}
	

	recvBuffer.Clear();

	RegisterRecv();
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

	if (!SocketHelper::SetReuseAddress(socket, true))
		return false;

	if (SocketHelper::BindAny(socket, 0) == false)
		return false;

	connectEvent.Init();
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

	GetService()->AddSession(GetSession());

	OnConnected();
	RegisterRecv();
}




//수정
void Session::Send(shared_ptr<Sendbuffer> sendBuffer)
{
	//Lock을 잡고
	unique_lock<shared_mutex> lock(rwLock);

	//sendQueue에 넣기만하고 끝냄
	sendQueue.push(sendBuffer);

	//내가 처음 Send를 호출하는 스레드 라면
	if (sendRegistered.exchange(true) == false)
	{

		//등록을 내 스레드가
		RegisterSend();

	}

}


void Session::RegisterSend()
{
	if (!IsConnected())
		return;

	//sendEvent 초기화
	sendEvent.Init();
	//sendEvent의 owner는 나의 session
	sendEvent.owner = shared_from_this();

	int writeSize = 0;
	//sendQueue의 데이터가 남아 있지 않을 때까지 돌림
	while (!sendQueue.empty())
	{
		//sendQueue의 앞부분부터 pop시키기 위해서 앞부분 데이터 캐싱
		shared_ptr<Sendbuffer> sendBuffer = sendQueue.front();

		writeSize += sendBuffer->WriteSize();

	}

	//내일 수정

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
		break;
	case EventType::DISCONNECT:	 
		ProcessDisconnect();
		break;
	default:
		break;
	}

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

	RegisterDisconnect();
}

bool Session::RegisterDisconnect()
{
	disconnectEvent.Init();
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
	disconnectEvent.owner = nullptr;

	OnDisconnected();
	GetService()->RemoveSession(GetSession());
}

void Session::HandleError(int errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		printf("Error Code : %d\n", errorCode);
		break;
	}
}

