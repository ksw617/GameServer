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


//RECV Ãß°¡
class RecvEvent : public IocpEvent
{
public:
public:
	RecvEvent() : IocpEvent(EventType::RECV) {}
};
