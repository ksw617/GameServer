#pragma once
#include "IocpEvent.h" // 빨간줄 때문에 
class IocpObj
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred = 0) abstract;
};

