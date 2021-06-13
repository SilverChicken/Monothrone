//#include "PlayerType.h"
#include "Gamemode.h"
#include "Unit.h"
#include "Player.h"


Gamemode* gameRef = &Gamemode::getInstance();

PlayerType::PlayerType()
{
	//default constructor for Player to take over
}

PlayerType::PlayerType(int Npid, Map* mapo)
{
	//setup

	//PID will be fed in by main, based on #of players currently on server
	PID = Npid;
	energy = 0;
	crystal = 0;

	//Possibly update the GUI here?

	hasThrone = true;


	map = mapo; //get reference to the map


	gameRef->addPlayer(PID, this);  //Adds self to map references

}

PlayerType::~PlayerType()
{
	for (std::map<Unit*, bool>::iterator it = units.begin(); it != units.end(); ++it) {
		if (it->first) {
			delete(it->first);
		}

	}
}

int PlayerType::getPID()
{
	return PID;
}

int PlayerType::getCrystal()
{
	return crystal;
}

int PlayerType::getEnergy()
{
	return energy;
}


void PlayerType::update()
{
	for (auto it = units.begin(); it != units.end(); it++) {
		it->first->update(map); //Works bc virtual -> We don't need explicit types, just matching function definitions!
	}
}

void PlayerType::update2()
{
	for (auto it = units.begin(); it != units.end(); ) {
		it->first->update2(); //Apply damage and other stuff

		//check that the iterator is still valid, or if it was erase we need to not increment it
		if (it->first->getLifeState() == 3) {
			units.erase(it++);
		}
		else {
			it++;
		}

	}
}

bool PlayerType::checkHasThrone()
{
	return hasThrone;
}

bool PlayerType::incEnergy(int val)
{
	energy += val;
	return true;
}

bool PlayerType::incCrystal(int val)
{
	crystal += val;
	return true;
}

bool PlayerType::decEnergy(int val)
{
	if (val <= energy) {
		energy -= val;
		return true;
	}
	return false;
}

bool PlayerType::decCrystal(int val)
{
	if (val <= crystal) {
		crystal -= val;
		return true;
	}
	return false;
}

void PlayerType::destroyUnit(Unit* unit)
{
	//destroys and cleans up a unit you own
	if (unit->getOwner() == PID) {
		//Die animation already finished
		

		//Maybe this function has no use right.


		//remove from player units map will be done in the current update2

		//After this the unit should have exited the scope and be cleaned up!
		//unit->~Unit(); 

	}
}

void PlayerType::draw(unsigned int* texture, GLuint shaderprog)
{

}

void PlayerType::draw(unsigned int* texture, PlayerType* pl, GLuint shaderprog) //ONLY but Player as argument
{
	//All unit draws are the same!
	//Only draw your units

	Player* player = (Player*)pl;

	for (auto it = units.begin(); it != units.end(); it++) {
		if (!player->cull(it->first->getLoc())) {
			it->first->draw(texture[it->first->getTexLoc()], shaderprog);
		}
	}
}

void PlayerType::addUnit(Unit* newUnit)
{
	units[newUnit] = newUnit->isSelected();
}

void PlayerType::removeUnit(Unit* del)
{
	units.erase(del);
}
