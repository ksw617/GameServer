#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h"
#include "SendBuffer.h" // SendBuffer사용할 꺼임


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
		//수정
		ProcessSend(bytes);
		break;
	case IO_TYPE::DISCONNECT:
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

void Session::ProcessSend(int32 bytes)
{
	//밀어줌
	sendEvent.owner = nullptr;
	sendEvent.sendBuffers.clear();
	

	if (bytes == 0)
	{
		Disconnect(L"Send data is zero\n");
		return;
	}

	OnSend(bytes);

	//lock잡고
	lock_guard<mutex> guard(lock);
	//해당 큐가 비어 있다면
	if (sendQueue.empty())
	{
		//sendRegustered에 그냥 false 적용
		sendRegistered.store(false);

	}
	else
	{	//처리 해줘야 함
		RegisterSend();
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
	if (recvBuffer.OnWrite(bytes) == false)
	{
		Disconnect(L"OnWrite overflow");
		return;
	}


	int32 dataSize = recvBuffer.DataSize();
	int32 processLen = OnRecv(recvBuffer.ReadPos(), bytes);

	if(processLen < 0)
	{
		Disconnect(L"OnWrite overflow");
		return;
	}

	if (dataSize < processLen)
	{
		Disconnect(L"OnWrite overflow");
		return;
	}

	if (recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnWrite overflow");
		return;
	}

	recvBuffer.Clean();


	RegisterRecv();
}

void Session::ProcessDisConnect()
{
	disconnectEvent.owner = nullptr;
}

bool Session::Connect()
{
	return RegisterConnect();
}


void Session::Send(shared_ptr<SendBuffer> sendBuffer)
{

	//lock을 잡고
	{
		lock_guard<mutex> guard(lock);

		//senQueue에 추가
		sendQueue.push(sendBuffer);
	}//풀고

	if (sendRegistered.exchange(true) == false)
	{
		RegisterSend();
	}
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


void Session::RegisterSend()
{
	if (IsConnected() == false)
	{
		return;
	}

	//밀어버리고
	sendEvent.Init();
	//SendEvent의 owner 나 자신(Session)
	sendEvent.owner = shared_from_this();

	{
		//lock잡고
		lock_guard<mutex> guard(lock);

		int32 writeSize = 0;
		//sendQueue값이 있다면 while루프 값이 다빠져나갈때 까지 루프돌림
		while (!sendQueue.empty())
		{
			//젤 먼저 들어간 sendBuffer를 담아서
			shared_ptr<SendBuffer> sendBuffer = sendQueue.front();

			//해당 sendBuffer 사용한 데이터 만큼 추가
			writeSize += sendBuffer->WriteSize();

			//sendQueue에서 해당 sendBffer를 제거
			sendQueue.pop();
			//해당 이벤트에 sendBuffer를 담기
			sendEvent.sendBuffers.push_back(sendBuffer);

		}

	} // lock 해제


	//여러개 보내기 위해서
	vector<WSABUF> wsaBufs;
	//위에서 SendEvent에 넣어준 sendBuffer 갯수만큼 할당
	wsaBufs.reserve(sendEvent.sendBuffers.size());				   
	//SendEvent의 sendBuffers들을 루프 돌면서
	for (shared_ptr<SendBuffer> sendBuffer : sendEvent.sendBuffers )
	{
		//WSABUF에 값 넣기
		WSABUF dataBuf;
		dataBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		dataBuf.len = static_cast<ULONG>(sendBuffer->WriteSize());

		//wsaBufs에 설정한거 넣어 주기
		wsaBufs.push_back(dataBuf);
	}

	DWORD sendBytes = 0;
	//여러개 보낼거라 설정 변경
	if (SOCKET_ERROR == WSASend(socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & sendBytes, 0, &sendEvent, NULL))
	{
		int32 errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent.owner = nullptr;
			//해당 sendBuffers를 밀어줌
			sendEvent.sendBuffers.clear();
			//여기서 다 진행했으니까 sendRegistered값은 false
			sendRegistered.store(false);
		}

	}
	

}

bool Session::RegisterDisconnect()
{
	disconnectEvent.Init();
	disconnectEvent.owner = shared_from_this();

	if (false == (SocketHelper::DisconnectEx(socket, &disconnectEvent, TF_REUSE_SOCKET, NULL)))
	{

		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			disconnectEvent.owner = nullptr;
			return false;

		}
	}

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
	dataBuf.buf = reinterpret_cast<char*>(recvBuffer.WritePos());
	dataBuf.len = recvBuffer.FreeSize();

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

Session::Session() : recvBuffer(4096)
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

	printf("Disconnect : %ws\n", cause);

	OnDisconnected();
	
	GetService()->RemoveSession(GetSession());

	RegisterDisconnect();
}
