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
	//SendEvent �����
	SendEvent* sendEvent = new SendEvent();

	//sendEvent�� ������ �� �ڽ�
	sendEvent->owner = this;

	//sendEvent�� vector<Byte>�� ũ�Ⱚ ����
	sendEvent->sendBuffer.resize(len);

	//�������ٰ� buffer�� ������ ����
	memcpy(sendEvent->sendBuffer.data(), buffer, len);

	//lock�� ���
	unique_lock<shared_mutex> lock(rwLock);

	//������ ����ϱ�
	RegisterSend(sendEvent);
}


void Session::RegisterSend(SendEvent* sendEvent)
{
	//���� ���°� �ƴ϶�� �� ����
	if (!IsConnected())
		return;

	//WSASend ����
	WSABUF wsaBuf;
	//sendEvent�� �ִ� ���� �����Ϳ� ũ��
	wsaBuf.buf = (char*)sendEvent->sendBuffer.data();
	wsaBuf.len = sendEvent->sendBuffer.size();

	DWORD sendLen = 0;
	DWORD flags = 0;

	//WSASend ȣ��
	if (WSASend(socket, &wsaBuf, 1, &sendLen, flags, sendEvent, nullptr) == SOCKET_ERROR)
	{
		//���� �ڵ� �ޱ�
		int errorCode = WSAGetLastError();
		//���� �߻���
		if (errorCode != WSA_IO_PENDING)
		{
			//���� ǥ��
			HandleError(errorCode);
			//sendEvent�� ������ ����
			sendEvent->owner = nullptr;

			//sendEvent ����
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
	printf("����\n");
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

