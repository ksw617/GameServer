#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h" // Protobuf에서 생성된 헤더 파일

// 패킷 ID 정의
enum : uint16
{
    // 로그인 관련 패킷 (1000번대)
    LOGIN_REQUEST = 1001,
    LOGIN_RESPONSE = 1002,

    // 게임 입장 관련 패킷 (1100번대)
    ENTER_GAME_REQUEST = 1101,
    ENTER_GAME_RESPONSE = 1102,

    // 플레이어 이동 관련 패킷 (1200번대)
    PLAYER_MOVE_REQUEST = 1201,
    PLAYER_MOVE_RESPONSE = 1202,

    // 채팅 관련 패킷 (1300번대)
    CHAT_REQUEST = 1301,
    CHAT_RESPONSE = 1302,

    // 액션 관련 패킷 (1400번대)
    ACTION_REQUEST = 1401,
    ACTION_RESPONSE = 1402,

    // 월드 상태 관련 패킷 (1500번대)
    WORLD_STATE_UPDATE = 1501,
};

// Recv 함수 정의
bool Handle_LoginRequest(shared_ptr<PacketSession>& session, Protocol::LoginRequest& packet);
bool Handle_EnterGameRequest(shared_ptr<PacketSession>& session, Protocol::EnterGameRequest& packet);
bool Handle_PlayerMoveRequest(shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet);
bool Handle_ChatRequest(shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet);
bool Handle_ActionRequest(shared_ptr<PacketSession>& session, Protocol::ActionRequest& packet);

class ServerPacketHandler : public PacketHandler
{
public:
    // Recv 함수 등록
    static void Init();

    // Send 함수 정의
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::LoginResponse& packet)
    {        return PacketHandler::MakeSendBuffer(packet, LOGIN_RESPONSE);    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::EnterGameResponse& packet) 
    {        return PacketHandler::MakeSendBuffer(packet, ENTER_GAME_RESPONSE);    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::PlayerMoveResponse& packet)
    {        return PacketHandler::MakeSendBuffer(packet, PLAYER_MOVE_RESPONSE);    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ChatResponse& packet)
    {        return PacketHandler::MakeSendBuffer(packet, CHAT_RESPONSE);    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ActionResponse& packet) 
    {        return PacketHandler::MakeSendBuffer(packet, ACTION_RESPONSE);    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::WorldStateUpdate& packet)
    {        return PacketHandler::MakeSendBuffer(packet, WORLD_STATE_UPDATE);    }
};
