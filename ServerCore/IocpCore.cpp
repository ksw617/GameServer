#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore::IocpCore()
{
	iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	if (iocpHandle != INVALID_HANDLE_VALUE)
	{
		CRASH("iocp Invalid");
	}
}

IocpCore::~IocpCore()
{
	CloseHandle(iocpHandle);
}

HANDLE IocpCore::GetHandle()
{
	return iocpHandle;
}

bool IocpCore::Register(IocpObj* iocpObj)
{
	return CreateIoCompletionPort(iocpObj->GetHandle(), iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObj), NULL);
}

bool IocpCore::Observe(uint32 time)
{
	DWORD bytes = 0;
	IocpObj* iocpObj = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (GetQueuedCompletionStatus(iocpHandle, OUT & bytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObj), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time))
	{
		iocpObj->Observe(iocpEvent, bytes);
	}
	else
	{
		switch (WSAGetLastError())
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			iocpObj->Observe(iocpEvent, bytes);
			break;
		}
	}

	return true;
}
