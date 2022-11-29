#include "pch.h"
#include "IocpEvent.h"

void IocpEvent::Init()
{
    OVERLAPPED::hEvent = NULL;
    OVERLAPPED::Internal = NULL;
    OVERLAPPED::InternalHigh = NULL;
    OVERLAPPED::Offset = NULL;
    OVERLAPPED::OffsetHigh = NULL;
    OVERLAPPED::Pointer = NULL;

}

IO_TYPE IocpEvent::GetType()
{
    return type;
}

IocpEvent::IocpEvent(IO_TYPE _type)
{
    Init();                                          
    type = _type;
}
