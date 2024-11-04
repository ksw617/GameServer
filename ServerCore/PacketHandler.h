#pragma once
#include "PacketSession.h"
#include "SendBufferManager.h"
#include <functional>

bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len);

class PacketHandler
{
protected:
	using PacketFunc = function<bool(shared_ptr<PacketSession>&, BYTE*, int)>;
	static PacketFunc packetHandlers[UINT16_MAX];
public:
	static void Init();
	static bool HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int len);
protected:
    template<typename PacketType, typename ProcessFunc>
    static bool HandlePacket(ProcessFunc func, shared_ptr<PacketSession>& session, BYTE* buffer, int len)
    {
        PacketType packet;
        packet.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader));

        return func(session, packet);
    }
protected:
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

