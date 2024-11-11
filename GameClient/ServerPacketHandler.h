#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h"

enum : uint16
{
    C_LOGIN = 0,
    S_LOGIN = 1,
    C_ENTER_GAME = 2,
    S_ENTER_GAME = 3,
    C_CHAT = 4,
    S_CHAT = 5,
    S_JOIN_GAME = 6,
};

bool Handle_S_LOGIN(shared_ptr<PacketSession>& session, Protocol::S_Login& packet);
bool Handle_S_ENTER_GAME(shared_ptr<PacketSession>& session, Protocol::S_EnterGame& packet);
bool Handle_S_CHAT(shared_ptr<PacketSession>& session, Protocol::S_Chat& packet);
bool Handle_S_JOIN_GAME(shared_ptr<PacketSession>& session, Protocol::S_JoinGame& packet);


class ServerPacketHandler : public PacketHandler
{
public:
    static void Init();

    //Send
public:
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_Login& packet) { return PacketHandler::MakeSendBuffer(packet, C_LOGIN); }
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_EnterGame& packet) { return PacketHandler::MakeSendBuffer(packet, C_ENTER_GAME); }
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_Chat& packet) { return PacketHandler::MakeSendBuffer(packet, C_CHAT); }

};

