#include "pch.h"
#include "ServerSession.h"

void ServerSession::OnConnected()
{

}

int ServerSession::OnRecvPacket(BYTE* buffer, int len)
{
    printf("%s\n", &buffer[4]);
    return len;
}

void ServerSession::OnSend(int len)
{
}

void ServerSession::OnDisconnected()
{
}
