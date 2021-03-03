#include "Gamemode.h"

Gamemode::~Gamemode()
{
}

Gamemode & Gamemode::getInstance()
{
	static Gamemode instance;
	return instance;
}

Unit * Gamemode::getThrone(int)
{
	return nullptr;
}

Player * Gamemode::getPlayer(int)
{
	return nullptr;
}

void Gamemode::addThrone(int, Unit *)
{
}

void Gamemode::addPlayer(int, Player *)
{
}
