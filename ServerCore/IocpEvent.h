#pragma once

enum class EventType : uint8
{
	CONNECT,
	DISCONNECT,
	ACCEPT,
	RECV,
	SEND,

};

class IocpEvent : public OVERLAPPED
{
public:
	EventType eventType;
	class IocpObj* owner = nullptr;
public:
	IocpEvent(EventType type);
public:
	void Init();

};

class AcceptEvent : public IocpEvent
{
public:
	class Session* session = nullptr;
public:
	AcceptEvent() : IocpEvent(EventType::ACCEPT) {}
};


class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::RECV) {}
};

//Send Ãß°¡
class SendEvent : public IocpEvent
{
public:
	vector<BYTE> sendBuffer;
public:
	SendEvent() : IocpEvent(EventType::SEND) {}
};
