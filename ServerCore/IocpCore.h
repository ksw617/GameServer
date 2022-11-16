#pragma once

class IocpObj
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Observe(class IocpEvent* iocpEvent, int32 bytes = 0) abstract;

};
class IocpCore
{
private:
	HANDLE iocpHandle;
public:
	IocpCore();
	~IocpCore();
public:
	HANDLE GetHandle();
	bool Register(class IocpObj* iocpObj);
	bool Observe(uint32 time = INFINITY);
};

