#pragma once

enum class EventType : uint8
{
	CONNECT,
	DISCONNECT,
	ACCEPT,
	RECV,
	SEND,

};

//���漱��
class IocpObj;
class Session;

class IocpEvent : public OVERLAPPED
{
public:
	EventType eventType;
	//shared_ptr<class IocpObj> owner;
	//class IocpObj* owner = nullptr;
	//����Ʈ ������ ���
	shared_ptr<IocpObj> owner;
public:
	IocpEvent(EventType type);
public:
	void Init();

};

class AcceptEvent : public IocpEvent
{
public:
	//class Session* session = nullptr;
	//����Ʈ ������ ���
	shared_ptr<Session> session;
public:
	AcceptEvent() : IocpEvent(EventType::ACCEPT) {}
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::CONNECT) {}
};


class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::DISCONNECT) {}
};


class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::RECV) {}
};

class SendEvent : public IocpEvent
{
public:
	vector<BYTE> sendBuffer;
public:
	SendEvent() : IocpEvent(EventType::SEND) {}
};
