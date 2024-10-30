#pragma once
#include <PacketSession.h>
#include <SendBufferManager.h>
#include "Protocol.pb.h"

enum : uint16
{
	C_LOGIN = 1001,
	S_LOGIN = 1002,

};

bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len);
bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& packet);

class ClientPacketHandler
{
    //Recv
public:
    using PacketFunc = function<bool(shared_ptr<PacketSession>&, BYTE*, int)>;
    //[0] -> Handle_INVALID();
    //[1] -> Handle_INVALID();
    //[2] -> Handle_INVALID();
    // .
    //[1001]-> Handle_C_LOGIN();
    // .
    // .
    //[65534]->Handle_INVALID();

    //UINT16_MAX == 65,535
    static PacketFunc packetHandlers[UINT16_MAX];
public:
    static void Init();
    static bool HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int len);
private:
    template<typename PacketType, typename ProcessFunc>
    static bool HandlePacket(ProcessFunc func, shared_ptr<PacketSession>& session, BYTE* buffer, int len)
    {
        PacketType packet;
        packet.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader));

        return func(session, packet);
    }
    

    //Send
public:
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_LOGIN& packet) { return MakeSendBuffer(packet, S_LOGIN); }
private:
	template<typename T>
	static shared_ptr<SendBuffer> MakeSendBuffer(T& packet, uint16 id)
	{
        uint16 dataSize = (uint16)packet.ByteSizeLong();
        uint16 packetSize = dataSize + sizeof(PacketHeader);   //dataSize + 4byte

        shared_ptr<SendBuffer> sendBuffer = SendBufferManager::Get().Open(4096);
        PacketHeader* buffer = (PacketHeader*)sendBuffer->GetBuffer();

        buffer->size = packetSize;
        buffer->id = id;  // 1

        if (!packet.SerializePartialToArray(&buffer[1], dataSize))
        {
            sendBuffer->Close(0);
            return nullptr;
        }

        if (!sendBuffer->Close(packetSize))
        {
            return nullptr;
        }

        return sendBuffer;
	}
};

