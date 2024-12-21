#include "pch.h"
#include "ServerPacketHandler.h"


void ServerPacketHandler::Init() {
	
    PacketHandler::Init();

	RegisterPacketHandler<Protocol::LoginRequest>(LOGIN_REQUEST, Handle_LoginRequest);
	RegisterPacketHandler<Protocol::EnterGameRequest>(ENTER_GAME_REQUEST, Handle_EnterGameRequest);
	RegisterPacketHandler<Protocol::PlayerMoveRequest>(PLAYER_MOVE_REQUEST, Handle_PlayerMoveRequest);
	RegisterPacketHandler<Protocol::ChatRequest>(CHAT_REQUEST, Handle_ChatRequest);
	RegisterPacketHandler<Protocol::ActionRequest>(ACTION_REQUEST, Handle_ActionRequest);
}


bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
    return false;
}


bool Handle_LoginRequest(shared_ptr<PacketSession>& session, Protocol::LoginRequest& packet) {

    return true;
}

bool Handle_EnterGameRequest(shared_ptr<PacketSession>& session, Protocol::EnterGameRequest& packet) {

    // 贸府 肺流
    return true;
}

bool Handle_PlayerMoveRequest(shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet) {

    // 贸府 肺流
    return true;
}

bool Handle_ChatRequest(shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet) {
    printf("ChatRequest received: message=%s\n", packet.message().c_str());
    // 贸府 肺流
    return true;
}

bool Handle_ActionRequest(shared_ptr<PacketSession>& session, Protocol::ActionRequest& packet) {

    return true;
}
