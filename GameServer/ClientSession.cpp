#include "pch.h"
#include "ClientSession.h"
#include "SessionManager.h"

void ClientSession::OnConnected()
{
    SessionManager::Get().Add(GetClientSession());
}

int ClientSession::OnRecvPacket(BYTE* buffer, int len)
{
    return len;
}

void ClientSession::OnSend(int len)
{
}

void ClientSession::OnDisconnected()
{
    SessionManager::Get().Remove(GetClientSession());
}