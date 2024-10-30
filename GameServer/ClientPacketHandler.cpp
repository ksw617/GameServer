#include "pch.h"
#include "ClientPacketHandler.h"

ClientPacketHandler::PacketFunc ClientPacketHandler::packetHandlers[UINT16_MAX];

void ClientPacketHandler::Init()
{

	for (int i = 0; i < UINT16_MAX; i++)
	{
		packetHandlers[i] = Handle_INVALID;

	}

	//C_LOGIN == 1001 == ID
	packetHandlers[C_LOGIN] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len)
		{
			return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len);

		};

}

bool ClientPacketHandler::HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
	PacketHeader* header = (PacketHeader*)buffer;

	return packetHandlers[header->id](session, buffer, len);
}

bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
	printf("Invalid handle\n");
	return false;
}

bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& packet)
{
	printf("Client Login\n");

	//Todo
	return true;
}
