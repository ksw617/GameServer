#pragma once
#include "ClientSession.h"

class Player : public enable_shared_from_this<Player>
{
public:
	uint64 id;
	string name;
public:
	weak_ptr<ClientSession> session;
public:
	Player() {}
	virtual ~Player() {}
};

