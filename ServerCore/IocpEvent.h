#pragma once

enum class IO_TYPE : uint8
{
	CONNECT,
	ACCEPT,
	RECV,
	SEND,

};

class IocpEvent	  : public OVERLAPPED
{
private:
	IO_TYPE type;
public:
	IocpEvent(IO_TYPE _type);
public:
	void Init();
	IO_TYPE GetType();

};

