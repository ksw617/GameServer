#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetworkAddress.h"

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	BYTE recvBuffer[1024] = {};
private:
	ConnectEvent connectEvent;
	//DisconnectEvent 추가
	DisconnectEvent disconnectEvent;
	RecvEvent recvEvent;
private:
	mutex lock;	
	shared_ptr<class Service> service; 

	SOCKET socket = INVALID_SOCKET;
	NetworkAddress networkAddress = {};
	atomic<bool> connected = false;
public:
	virtual HANDLE GetHandle() override;
	virtual void Observe(class IocpEvent* iocpEvent, int32 bytes = 0) override;

public:
	bool Connect();
	void Send(BYTE* buffer, int32 len);
	void Disconnect(const WCHAR* cause);

public:
	void SetService(shared_ptr<Service> _service) { service = _service; }
	shared_ptr<Service> GetService() { return service; }

	SOCKET GetSocket() { return socket;	}
	void SetNetworkAddress(NetworkAddress address){ networkAddress = address; }
	NetworkAddress GetNetworkAddress(){return networkAddress; }
	bool IsConnected() { return connected; }

	shared_ptr<Session> GetSession() { return static_pointer_cast<Session>(shared_from_this()); }
private:
	bool RegisterConnect();
	void RegisterRecv();
	void RegisterSend(SendEvent* sendEvent);
	//추가
	bool RegisterDisconnect();

	void ProcessConnect();
	void ProcessRecv(int32 bytes);
	void ProcessSend(SendEvent* sendEvent, int32 bytes);
	//추가
	void ProcessDisConnect();

	void HandleError(int32 error);

protected:
	virtual void OnConnected() {}
	virtual void OnDisconnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
public:
	Session();
	~Session();
};

