#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"


HANDLE IocpCore::GetHandle()
{
	return iocpHandle;
}

bool IocpCore::Register(IocpObject* iocpObj)
{
	//key´Â NULL·Î
	return CreateIoCompletionPort(iocpObj->GetHandle(), iocpHandle, NULL, NULL);
}

bool IocpCore::Observe(uint32 time)							
{					
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	if (GetQueuedCompletionStatus(iocpHandle,OUT &bytesTransferred,OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time))
	{
		shared_ptr<IocpObject> iocpObj = iocpEvent->owner;
		iocpObj->Observe(iocpEvent, bytesTransferred);
	}
	else
	{
		switch (WSAGetLastError())
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			shared_ptr<IocpObject> iocpObj = iocpEvent->owner;
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
