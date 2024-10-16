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
	//exchage�� ���� isConnected�� �鱸 �ִ� ���� ��ȯ
	//isConncted�� ���� true�� ��Ȳ�϶��� false�� ��ȯ �����ְ� ���� ��� �ִ�true�� ��ȯ
	//�״� ���ʹ� isConncted == flase�ϱ� ��� flase ��ȯ
	if (isConnected.exchange(false) == false)
		return;

	//�� ���� ������� ���� ǥ��
	wprintf(L"Disconnect reason : $ls\n", cause);

	//���� ���� ���� ���� ServerSession�� �ִ� OnDisconnected ȣ��
	OnDisconnected();

	//���� �ݾ��ְ�
	SocketHelper::CloseSocket(socket);

	//���� Service���� ���� session�� ����
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
		//������ ���ٰ� ���� ����
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

