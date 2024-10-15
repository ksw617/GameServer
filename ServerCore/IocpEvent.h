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
	//나를 들고 있는 주체가 누구인지
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

