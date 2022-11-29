#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore GIocpCore;

HANDLE IocpCore::GetHandle()
{
	return iocpHandle;
}

bool IocpCore::Register(IocpObject* iocpObj)
{
	return CreateIoCompletionPort(iocpObj->GetHandle(), iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObj), NULL);
}

bool IocpCore::Observe(uint32 time)							
{
	DWORD bytesTransferred = 0;
	IocpObject* iocpObj = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, reinterpret_cast<ULONG_PTR*>(&iocpObj), reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time))
	{
		
		iocpObj->Observe(iocpEvent, bytesTransferred);
	}
	else
	{
		switch (WSAGetLastError())
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			iocpObj->Observe(iocpEvent, bytesTransferred);
			break;
		}
	}

	return true;
}

IocpCore::IocpCore()
{
	iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
	CONDITION_CRASH(iocpHandle != NULL);
}

IocpCore::~IocpCore()
{
	CloseHandle(iocpHandle);
}
