#include "pch.h"
#include "GameRoom.h"
#include "Player.h"

void GameRoom::Enter(shared_ptr<Player> player)
{
	unique_lock<shared_mutex> lock(rwLock);
	players.insert(player);
	printf("Player ID [%u] ÀÔÀå\n", player->id);

}

void GameRoom::Leave(shared_ptr<Player> player)
{
	unique_lock<shared_mutex> lock(rwLock);
	players.erase(player);
}

void GameRoom::Broadcast(shared_ptr<class SendBuffer> sendBuffer)
{
	unique_lock<shared_mutex> lock(rwLock);
	for (auto& player : players)
	{
		auto session = player->session.lock();
		if (session)
		{
			session->Send(sendBuffer);
		}

	}
}
