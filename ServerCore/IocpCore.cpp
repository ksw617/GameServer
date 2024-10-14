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
    //key�� �� ��� �Ҳ��� 0
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

        //Session�ϰ� Listener�� IocpObj�� ��ӹ��� ����
        IocpObj* iocpObj = iocpEvent->owner;
        //iocpObj�� ObserveIO�� �����Լ��̱� ������
        //1. iocpObj�� �Ҵ�� ���̰� Session�̶�� Session->ObserveIO�� ����
        //2. iocpObj�� �Ҵ�� ���̰� Listener��� Listenr->ObserveIO�� ����
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
            //GetQueuedCompletionStatus ��ٸ��� �ð��� �Ѿ�Ŵϱ�
            return false;
        default:
            break;
        }
        return false;
    }

    return true;

}
