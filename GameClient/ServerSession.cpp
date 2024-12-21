#include "pch.h"
#include "ServerSession.h"

#include "Protocol.pb.h"
#include "ClientPacketHandler.h"

void ServerSession::OnConnected()
{
    Protocol::LoginRequest packet;
    auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
    Send(sendBuffer);
    
}

int ServerSession::OnRecvPacket(BYTE* buffer, int len)
{
    shared_ptr<PacketSession> session = GetPacketSession();
    ClientPacketHandler::HandlePacket(session, buffer, len);

    return len;
}

void ServerSession::OnSend(int len)
{
}

void ServerSession::OnDisconnected()
{
}
