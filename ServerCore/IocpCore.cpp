#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "IocpObj.h"

IocpCore::IocpCore()
{
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
}

IocpCore::~IocpCore()
{
    CloseHandle(iocpHandle);
}

void IocpCore::Register(IocpObj* iocpObj)
{
    //iocpObj->GetHandle() : return (HANDLE)socket
    //key는 안 사용 할꺼라 0
    CreateIoCompletionPort(iocpObj->GetHandle(), iocpHandle, 0, 0);

}

bool IocpCore::ObserveIO(DWORD time)
{
    DWORD bytesTransferred = 0;
    ULONG_PTR key = 0;
    IocpEvent* iocpEvent = nullptr;

    printf("Waiting....\n");
    if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&iocpEvent, INFINITE))
    {

        //Session하고 Listener는 IocpObj를 상속받을 꺼임
        IocpObj* iocpObj = iocpEvent->owner;
        //iocpObj의 ObserveIO는 가상함수이기 때문에
        //1. iocpObj가 할당된 아이가 Session이라면 Session->ObserveIO를 실행
        //2. iocpObj가 할당된 아이가 Listener라면 Listenr->ObserveIO를 실행
        iocpObj->ObserveIO(iocpEvent, bytesTransferred);

        

        //switch (iocpEvent->eventType)
        //{
        //case EventType::ACCEPT:
        //    printf("Client connected....\n");
        //    break;
        //
        //default:
        //    break;
        //}
    }
    else
    {
        //printf("GetQueuedCompletionStatus function failed with error : %d\n", WSAGetLastError());
        
        switch (GetLastError())
        {
        case WAIT_TIMEOUT:
            //GetQueuedCompletionStatus 기다리는 시간을 넘어간거니까
            return false;
        default:
            break;
        }
        return false;
    }

    return true;

}
