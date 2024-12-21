#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h"

// 서버 전용 패킷 ID
enum : uint16_t
{
    LOGIN_REQUEST = 1001,
    ENTER_GAME_REQUEST = 1003,
    PLAYER_MOVE_REQUEST = 1005,
    CHAT_REQUEST = 1007,
    ACTION_REQUEST = 1009,
};

// 클라이언트 요청을 처리하는 핸들러 함수 선언
bool Handle_LoginRequest(shared_ptr<PacketSession>& session, Protocol::LoginRequest& packet);
bool Handle_EnterGameRequest(shared_ptr<PacketSession>& session, Protocol::EnterGameRequest& packet);
bool Handle_PlayerMoveRequest(shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet);
bool Handle_ChatRequest(shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet);
bool Handle_ActionRequest(shared_ptr<PacketSession>& session, Protocol::ActionRequest& packet);

class ClientPacketHandler : public PacketHandler
{
public:
    static void Init(); // 핸들러 초기화

    // Send 함수 정의 (서버 → 클라이언트 응답)
    // LOGIN_RESPONSE = LOGIN_REQUEST + 1
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::LoginResponse& packet) 
    {        return PacketHandler::MakeSendBuffer(packet, LOGIN_REQUEST + 1);     }

    // ENTER_GAME_RESPONSE
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::EnterGameResponse& packet) 
    {        return PacketHandler::MakeSendBuffer(packet, ENTER_GAME_REQUEST + 1);    }

    // PLAYER_MOVE_RESPONSE
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::PlayerMoveResponse& packet) 
    {        return PacketHandler::MakeSendBuffer(packet, PLAYER_MOVE_REQUEST + 1);    }

    // CHAT_RESPONSE
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ChatResponse& packet) 
    {        return PacketHandler::MakeSendBuffer(packet, CHAT_REQUEST + 1);    }

    // ACTION_RESPONSE
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ActionResponse& packet) 
    {        return PacketHandler::MakeSendBuffer(packet, ACTION_REQUEST + 1);    }
};