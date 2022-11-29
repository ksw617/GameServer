#pragma once

class IocpObject : public enable_shared_from_this<IocpObject>
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
	HANDLE GetHandle();
	bool Register(shared_ptr<IocpObject> iocpObj);
	bool Observe(uint32 time = INFINITY);
public:
	IocpCore();
	~IocpCore();
};

