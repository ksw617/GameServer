#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetworkAddress.h"
#include "RecvBuffer.h"		   //�߰�

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	RecvBuffer recvBuffer;

	//�۽Ű���
	queue<shared_ptr<SendBuffer>> sendQueue;
	atomic<bool> sendRegistered = false;
private:
	ConnectEvent connectEvent;
	DisconnectEvent disconnectEvent;
	RecvEvent recvEvent;
	//SendEvent ����Ҳ��� 
	SendEvent sendEvent;
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
	//Send��� ����
	void Send(shared_ptr<SendBuffer> sendBuffer);
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
	//SendEvent �Ҵ��Ҳ��� �ȳѰ��൵ ��
	void RegisterSend();
	bool RegisterDisconnect();

	void ProcessConnect();
	void ProcessRecv(int32 bytes);
	//SendEvent �Ҵ��Ҳ��� �ȳѰ��൵ ��
	void ProcessSend(int32 bytes);
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

