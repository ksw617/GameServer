#pragma once
#include "IocpObj.h"

class Service;

class Session : public IocpObj
{
	friend class Listener;

private:
	shared_mutex rwLock;	// 멀티스레드 용
	atomic<bool> isConnected = false;
	Service* service = nullptr;
	SOCKET socket = INVALID_SOCKET;
	SOCKADDR_IN sockAddr = {};
private:
	RecvEvent recvEvent;
public:
	BYTE recvBuffer[1024] = {};
public:
	Session();
	virtual ~Session();
public:
	SOCKET GetSocket() const { return socket; }
	HANDLE GetHandle() override { return (HANDLE)socket; };
	Service* GetService() const { return service; }
	bool IsConnected() const { return isConnected; }
public:
	void SetSockAddr(SOCKADDR_IN address) { sockAddr = address; }
	void SetService(Service* _service) { service = _service; }
private:
	void RegisterRecv();
	//Send 등록
	void RegisterSend(SendEvent* sendEvent);
private:
	void ProcessConnect();
	void ProcessRecv(int bytesTransferred);
	//Send 진행
	void ProcessSend(SendEvent* sendEvent, int bytesTransferred);
private:
	void HandleError(int errorCode);
public:
	virtual void OnConnected() {}
	virtual int OnRecv(BYTE* buffer, int len) { return len; }
	//상속 받은애 OnSend 호출
	virtual void OnSend(int len) {}
	virtual void OnDisconnected() {}
public:
	//실제로 데이터 보내는 용도
	void Send(BYTE* buffer, int len);
	void Disconnect(const WCHAR* cause);
public:
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;

};

