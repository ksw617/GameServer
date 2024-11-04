#include "pch.h"
#include "ServerPacketHandler.h"



void ServerPacketHandler::Init()
{
	PacketHandler::Init();

	packetHandlers[S_LOGIN] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len)
		{ return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len);	 };
	packetHandlers[S_ENTER_GAME] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len)
		{ return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, buffer, len);	 };
	packetHandlers[S_CHAT] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int len)
		{ return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, session, buffer, len);	 };
}


bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
	printf("Invalid handle\n");
	return false;
}

bool Handle_S_LOGIN(shared_ptr<PacketSession>& session, Protocol::S_LOGIN& packet)
{

	if (!packet.success())
	{
		session->Disconnect(L"Refused");
		return false;
	}

	printf("Connected\n");
	Protocol::C_ENTER_GAME sendPacket;

	Protocol::Player* player = new Protocol::Player;
	player->set_id(packet.playerid());
	player->set_name("TEST");

	sendPacket.set_allocated_player(player);
										   	

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(sendPacket);
	session->Send(sendBuffer);


	return true;
}

bool Handle_S_ENTER_GAME(shared_ptr<PacketSession>& session, Protocol::S_ENTER_GAME& packet)
{
	return false;
}

bool Handle_S_CHAT(shared_ptr<PacketSession>& session, Protocol::S_CHAT& packet)
{
	return false;
}
