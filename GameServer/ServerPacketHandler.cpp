#include "pch.h"
#include "ServerPacketHandler.h"

// 패킷 핸들러 초기화 (서버 부팅 시 1회 호출)
void ServerPacketHandler::Init()
{
    // 요청 패킷에 대한 핸들러 등록
    RegisterPacketHandler<Protocol::LoginRequest>(LOGIN_REQUEST, Handle_LoginRequest);
    RegisterPacketHandler<Protocol::EnterGameRequest>(ENTER_GAME_REQUEST, Handle_EnterGameRequest);
    RegisterPacketHandler<Protocol::PlayerMoveRequest>(PLAYER_MOVE_REQUEST, Handle_PlayerMoveRequest);
    RegisterPacketHandler<Protocol::ChatRequest>(CHAT_REQUEST, Handle_ChatRequest);
    RegisterPacketHandler<Protocol::ActionRequest>(ACTION_REQUEST, Handle_ActionRequest);
}

// 클라이언트에서 로그인 요청 패킷을 받았을 때 처리
bool ServerPacketHandler::Handle_LoginRequest(shared_ptr<PacketSession>& session, Protocol::LoginRequest& packet)
{
    printf("Login request from: %s\n", packet.username().c_str());

}

// 게임 입장 요청
bool ServerPacketHandler::Handle_EnterGameRequest(shared_ptr<PacketSession>& session, Protocol::EnterGameRequest& packet)
{

    return true;
}

// 플레이어 이동 요청 처리
bool ServerPacketHandler::Handle_PlayerMoveRequest(shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet)
{

    return true;
}

// 채팅 요청 처리
bool ServerPacketHandler::Handle_ChatRequest(shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet)
{

    return true;
}

// 액션 요청 처리
bool ServerPacketHandler::Handle_ActionRequest(shared_ptr<PacketSession>& session, Protocol::ActionRequest& packet)
{

    return true;
}
