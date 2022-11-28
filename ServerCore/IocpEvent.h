#pragma once

enum class IO_TYPE : uint8
{
	CONNECT,
	ACCEPT,
	RECV, 
	SEND,

};

class IocpEvent : public OVERLAPPED
{
protected:
	IO_TYPE type;
public:
	void Init();
	IO_TYPE GetType();
public:
	IocpEvent(IO_TYPE _type);
	shared_ptr<class IocpObject> owner;
};

class AcceptEvent : public IocpEvent
{
public:
	shared_ptr<class Session> session = nullptr;
public:
	AcceptEvent() : IocpEvent(IO_TYPE::ACCEPT) {}

};
