#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h"  // Protobuf으로 생성된 헤더 파일

// 패킷 ID 정의
enum : uint16
{
    LOGIN_REQUEST = 1001,
    ENTER_GAME_REQUEST = 1101,
    PLAYER_MOVE_REQUEST = 1201,
    CHAT_REQUEST = 1301,
    ACTION_REQUEST = 1401,
};

// 서버 → 클라이언트 응답 패킷 ID
enum : uint16
{
    LOGIN_RESPONSE = 1002,
    ENTER_GAME_RESPONSE = 1102,
    PLAYER_MOVE_RESPONSE = 1202,
    CHAT_RESPONSE = 1302,
    ACTION_RESPONSE = 1402,
    WORLD_STATE_UPDATE = 1501,
};

// 서버 패킷 핸들러 정의
class ServerPacketHandler : public PacketHandler
{
public:
    // 초기화 (패킷 핸들러 등록)
    static void Init();

    // 클라이언트 → 서버 요청 처리 핸들러
    static bool Handle_LoginRequest(shared_ptr<PacketSession>& session, Protocol::LoginRequest& packet);
    static bool Handle_EnterGameRequest(shared_ptr<PacketSession>& session, Protocol::EnterGameRequest& packet);
    static bool Handle_PlayerMoveRequest(shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet);
    static bool Handle_ChatRequest(shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet);
    static bool Handle_ActionRequest(shared_ptr<PacketSession>& session, Protocol::ActionRequest& packet);

    // 서버 → 클라이언트 응답 전송
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::LoginResponse& packet)
    {
        return PacketHandler::MakeSendBuffer(packet, LOGIN_RESPONSE);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::EnterGameResponse& packet)
    {
        return PacketHandler::MakeSendBuffer(packet, ENTER_GAME_RESPONSE);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::PlayerMoveResponse& packet)
    {
        return PacketHandler::MakeSendBuffer(packet, PLAYER_MOVE_RESPONSE);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ChatResponse& packet)
    {
        return PacketHandler::MakeSendBuffer(packet, CHAT_RESPONSE);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ActionResponse& packet)
    {
        return PacketHandler::MakeSendBuffer(packet, ACTION_RESPONSE);
    }
};
