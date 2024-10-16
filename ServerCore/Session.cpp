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

void Session::Disconnect(const WCHAR* cause)
{
	//isConncted = false;
	//exchage가 원래 isConnected가 들구 있는 값을 반환
	//isConncted가 최초 true인 상황일때만 false로 변환 시켜주고 원래 들고 있던true값 반환
	//그담 부터는 isConncted == flase니까 계속 flase 반환
	if (isConnected.exchange(false) == false)
		return;

	//왜 연결 끊어는지 원이 표시
	wprintf(L"Disconnect reason : $ls\n", cause);

	//아직 연결 끊기 전에 ServerSession에 있는 OnDisconnected 호출
	OnDisconnected();

	//소켓 닫아주고
	SocketHelper::CloseSocket(socket);

	//현재 Service에서 내꺼 session을 제거
	GetService()->RemoveSession(this);
}

void Session::ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred)
{
	switch (iocpEvent->eventType)
	{
	case EventType::RECV:
		ProcessRecv(byteTransferred);
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
		//받은거 없다고 연결 끊기
		Disconnect(L"Recv 0 bytes");
		return;
	}

	OnRecv(recvBuffer, bytesTransferred);

	RegisterRecv();
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

