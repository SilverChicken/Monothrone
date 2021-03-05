#include "Gamemode.h"
#include "Throne.h"
#include "Player.h"
#include "Ressource.h"
#include "ImLoader.h"


//unsigned int * ImLoader::textures; //The static array from ImLoader

Gamemode::~Gamemode()
{
//delete all the things

	for (int i = 0; i < MAX_PLAYER; i++) {
		if (Thrones.count(i)) {  //Get Thrones & Players
			delete Thrones[i];
		}
		if (Players.count(i)) {
			delete Players[i];    //This destructor will propagate to Units of Player
		}
	}


}

Gamemode & Gamemode::getInstance()
{
	static Gamemode instance;
	return instance;
}

std::vector<Ressource*> Gamemode::getMountains()
{
	return Mountains;
}

std::vector<Ressource*> Gamemode::getCrystals()
{
	return Crystals;
}

std::vector<Ressource*> Gamemode::getEnergies()
{
	return Energies;
}

bool Gamemode::addMountain(Ressource * res)
{
	if (res->getClassType() == MOUNTAIN_CLASS_T) {
		Mountains.push_back(res);
		return true;
	}
	return false;
}

bool Gamemode::addCrystals(Ressource * res)
{
	if (res->getClassType() == CRYSTAL_CLASS_T) {
		Crystals.push_back(res);
		return true;
	}
	return false;
}

bool Gamemode::addEnergies(Ressource * res)
{
	if (res->getClassType() == ENERGY_CLASS_T) {
		Energies.push_back(res);
		return true;
	}
	return false;
}

Unit * Gamemode::getThrone(int own)
{
	return Thrones[own];
}

Player * Gamemode::getPlayer(int own)
{
	return Players[own];
}

void Gamemode::addThrone(int own, Unit * throne)
{
	if (throne->getClassType() == THRONE_CLASS_T) { //Check that it's actually a throne
		Thrones[own] = throne;
	}
}

void Gamemode::addPlayer(int own, Player * player)
{
	Players[own] = player;
}
