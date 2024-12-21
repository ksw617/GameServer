#include "pch.h"
#include "ClientPacketHandler.h"

void ClientPacketHandler::Init()
{
    // 패킷 ID와 핸들러를 매핑 (서버 → 클라이언트 응답 패킷)
    RegisterPacketHandler<Protocol::LoginResponse>(LOGIN_RESPONSE, Handle_LoginResponse);
    RegisterPacketHandler<Protocol::EnterGameResponse>(ENTER_GAME_RESPONSE, Handle_EnterGameResponse);
    RegisterPacketHandler<Protocol::PlayerMoveResponse>(PLAYER_MOVE_RESPONSE, Handle_PlayerMoveResponse);
    RegisterPacketHandler<Protocol::ChatResponse>(CHAT_RESPONSE, Handle_ChatResponse);
    RegisterPacketHandler<Protocol::ActionResponse>(ACTION_RESPONSE, Handle_ActionResponse);
    RegisterPacketHandler<Protocol::ActionResponse>(WORLD_STATE_UPDATE, Handle_WorldStateUpdate);
}

// 서버로부터 받은 응답 처리
bool ClientPacketHandler::Handle_LoginResponse(shared_ptr<PacketSession>& session, Protocol::LoginResponse& packet)
{
    if (packet.success())
    {
        printf("Login successful! Session ID: %s\n", packet.session_id().c_str());
    }
    else
    {
        printf("Login failed: %s\n", packet.error_message().c_str());
    }
    return true;
}

bool ClientPacketHandler::Handle_EnterGameResponse(shared_ptr<PacketSession>& session, Protocol::EnterGameResponse& packet)
{
    printf("Entered game world. Welcome!\n");
    return true;
}

bool ClientPacketHandler::Handle_PlayerMoveResponse(shared_ptr<PacketSession>& session, Protocol::PlayerMoveResponse& packet)
{
    printf("Player move acknowledged.\n");
    return true;
}

bool ClientPacketHandler::Handle_ChatResponse(shared_ptr<PacketSession>& session, Protocol::ChatResponse& packet)
{
    return true;
}

bool ClientPacketHandler::Handle_ActionResponse(shared_ptr<PacketSession>& session, Protocol::ActionResponse& packet)
{
    printf("Action response received.\n");
    return true;
}

bool ClientPacketHandler::Handle_WorldStateUpdate(shared_ptr<PacketSession>& session, Protocol::ActionResponse& packet)
{
    printf("World state updated.\n");
    return true;
}
