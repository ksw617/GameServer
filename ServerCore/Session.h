#pragma once
#include "IocpObj.h"
//클라는 하나씩, 서버는 accpet 할때마다 생성
class Session : public IocpObj
{
private:
	SOCKET socket = INVALID_SOCKET;
	//클라이언트라면 서버의 주소
	//서버면 클라이언트의 주소
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
	//연결진행
	void ProcessConnect();
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;

};

