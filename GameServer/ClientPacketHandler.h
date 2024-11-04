#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h"
      
enum : uint16
{
	C_LOGIN = 1001,
	S_LOGIN = 1002,
    C_ENTER_GAME = 1003,
    S_ENTER_GAME = 1004,
    C_CHAT = 1005,
    S_CHAT = 1006,
};

//Recv 함수 정의
bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& packet);
bool Handle_C_ENTER_GAME(shared_ptr<PacketSession>& session, Protocol::C_ENTER_GAME& packet);
bool Handle_C_CHAT(shared_ptr<PacketSession>& session, Protocol::C_CHAT& packet);

class ClientPacketHandler : public PacketHandler
{

public:
    //Recv 함수 등록
    static void Init();
private:


    //Send 함수 정의
public:
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_LOGIN& packet) { return PacketHandler::MakeSendBuffer(packet, S_LOGIN); }
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_ENTER_GAME& packet) { return PacketHandler::MakeSendBuffer(packet, S_ENTER_GAME); }
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_CHAT& packet) { return PacketHandler::MakeSendBuffer(packet, S_CHAT); }
};

