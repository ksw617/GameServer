#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

//юс╫ц
IocpCore GIocpCore;

IocpCore::IocpCore()
{
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
    if (iocpHandle != INVALID_HANDLE_VALUE)
    {
        CRASH("iocpHandle");
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
    return CreateIoCompletionPort(iocpObj->GetHandle(), iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObj), NULL);;
}

bool IocpCore::Observe(uint32 time)
{
    DWORD bytesTransferred = 0;
    IocpObj* iocpObj = nullptr;
    IocpEvent* iocpEvent = nullptr;

    BOOL returnValue = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, reinterpret_cast<ULONG_PTR*>(&iocpObj), reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time);
    if (returnValue)
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
