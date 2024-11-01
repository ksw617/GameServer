#include "pch.h"
#include "ServerPacketHandler.h"

ServerPacketHandler::PacketFunc ServerPacketHandler::packetHandlers[UINT16_MAX];

void ServerPacketHandler::Init()
{
	for (int i = 0; i < UINT16_MAX; i++)
	{
		packetHandlers[i] = Handle_INVALID;

	}

	//C_LOGIN == 1001 == ID
	packetHandlers[S_LOGIN] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len)
		{
			return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len);

		};
}

bool ServerPacketHandler::HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
	PacketHeader* header = (PacketHeader*)buffer;
	return packetHandlers[header->id](session, buffer, len);
}


bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
	printf("Invalid handle\n");
	return false;
}

bool Handle_S_LOGIN(shared_ptr<PacketSession>& session, Protocol::S_LOGIN& packet)
{
	printf("Connected\n");

	//Todo
	return true;
}
