#pragma once
#include "IocpObj.h"
//Ŭ��� �ϳ���, ������ accpet �Ҷ����� ����
class Session : public IocpObj
{
private:
	SOCKET socket = INVALID_SOCKET;
	//Ŭ���̾�Ʈ��� ������ �ּ�
	//������ Ŭ���̾�Ʈ�� �ּ�
	SOCKADDR_IN sockAddr = {};
public:
	BYTE recvBuffer[1024] = {};
public:
	Session();
	virtual ~Session();
public:
	SOCKET GetSocket() const { return socket; }
	HANDLE GetHandle() override { return (HANDLE)socket; };
public:
	void SetSockAddr(SOCKADDR_IN address) { sockAddr = address; }
public:
	//��������
	void ProcessConnect();
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;

};

