#pragma once
class Session;

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
	IocpEvent(IO_TYPE _type);
public:
	void Init();
	IO_TYPE GetType();
};

class AcceptEvent : public IocpEvent
{
private:
	Session* session = nullptr;
public:
	void SetSession(Session* _session) { session = _session; }
	Session* GetSession() { return session; }
public:
	AcceptEvent() : IocpEvent(IO_TYPE::ACCEPT) {}

};

