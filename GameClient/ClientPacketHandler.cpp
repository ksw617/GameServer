#include "pch.h"
#include "ClientPacketHandler.h"

void ClientPacketHandler::Init()
{
    PacketHandler::Init();

    // 클라이언트 요청 패킷 핸들러 등록
    packetHandlers[LOGIN_REQUEST] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len)
        { return HandlePacket<Protocol::LoginRequest>(Handle_LoginRequest, session, buffer, len); };

    packetHandlers[ENTER_GAME_REQUEST] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len) 
        { return HandlePacket<Protocol::EnterGameRequest>(Handle_EnterGameRequest, session, buffer, len); };

    packetHandlers[PLAYER_MOVE_REQUEST] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len) 
        { return HandlePacket<Protocol::PlayerMoveRequest>(Handle_PlayerMoveRequest, session, buffer, len); };

    packetHandlers[CHAT_REQUEST] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len) 
        { return HandlePacket<Protocol::ChatRequest>(Handle_ChatRequest, session, buffer, len);  };

    packetHandlers[ACTION_REQUEST] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len) 
        {  return HandlePacket<Protocol::ActionRequest>(Handle_ActionRequest, session, buffer, len);  };
}

bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
    return false;
}


bool Handle_LoginRequest(shared_ptr<PacketSession>& session, Protocol::LoginRequest& packet)
{
    printf("LoginRequest received: username=%s\n", packet.username().c_str());


    return true;
}

bool Handle_EnterGameRequest(shared_ptr<PacketSession>& session, Protocol::EnterGameRequest& packet)
{

    return true;
}

bool Handle_PlayerMoveRequest(shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet)
{
    return true;
}

bool Handle_ChatRequest(shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet)
{
    printf("ChatRequest received: message=%s\n", packet.message().c_str());

    return true;
}

bool Handle_ActionRequest(shared_ptr<PacketSession>& session, Protocol::ActionRequest& packet)
{
    printf("ActionRequest received: action_id=%s\n", packet.action_id().c_str());

    return true;
}

