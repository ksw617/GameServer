#pragma once
#include "IocpObj.h"

class Service;

class Session : public IocpObj
{
	friend class Listener;

private:
	shared_mutex rwLock;	
	atomic<bool> isConnected = false;
	//스마트 포인터로 변환
	shared_ptr<Service> service = nullptr;
	SOCKET socket = INVALID_SOCKET;
	SOCKADDR_IN sockAddr = {};
private:
	ConnectEvent connectEvent;
	RecvEvent recvEvent;
	DisconnectEvent disconnectEvent;
public:
	BYTE recvBuffer[1024] = {};
public:
	Session();
	virtual ~Session();
public:
	SOCKET GetSocket() const { return socket; }
	HANDLE GetHandle() override { return (HANDLE)socket; };
	//스마트 포인터로 변환
	shared_ptr<Service> GetService() const { return service; }
	bool IsConnected() const { return isConnected; }
public:
	void SetSockAddr(SOCKADDR_IN address) { sockAddr = address; }
	//스마트 포인터로 변환
	void SetService(shared_ptr<Service> _service) { service = _service; }
private:
	bool RegisterConnect();
	void RegisterRecv();
	void RegisterSend(SendEvent* sendEvent);
	bool RegisterDisconnect();
private:
	void ProcessConnect();
	void ProcessRecv(int bytesTransferred);
	void ProcessSend(SendEvent* sendEvent, int bytesTransferred);
	//Disconnect 진행
	void ProcessDisconnect();
private:
	void HandleError(int errorCode);
public:
	virtual void OnConnected() {}
	virtual int OnRecv(BYTE* buffer, int len) { return len; }
	virtual void OnSend(int len) {}
	virtual void OnDisconnected() {}
public:
	bool Connect();
	void Send(BYTE* buffer, int len);
	void Disconnect(const WCHAR* cause);
public:
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;

};

