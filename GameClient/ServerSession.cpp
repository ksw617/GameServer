#include "pch.h"
#include "ServerSession.h"

#include "Protocol.pb.h"

void ServerSession::OnConnected()
{

}

int ServerSession::OnRecvPacket(BYTE* buffer, int len)
{
    Protocol::TEST packet;
    //&buffer[4], 실제 길이에서 - 4
    packet.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader));

    printf("ID : %d, HP : %d, MP : %d\n", packet.id(), packet.hp(), packet.mp());

    return len;
}

void ServerSession::OnSend(int len)
{
}

void ServerSession::OnDisconnected()
{
}
