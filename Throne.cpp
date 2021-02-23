#include "Throne.h"
#include "Map.h"
#include "Player.h"


Throne::Throne(int owner, Location * loc, Map * map):Unit(owner, loc, map)
{
	hp = 25;
	atk = 0;
	speed = 0;
	textLoc = textLocs[owner]; 

	classType = 5;

	map->addThrone(owner, this);
	playerRef = map->getPlayer(owner);            //Forces include Map.h, and relies on Player being created before.

	//Define which actions are defined
	actions[0] = false;
	actions[1] = false;
	actions[2] = true;
	actions[3] = true;
	actions[4] = true;
	actions[5] = false;
}

Throne::~Throne()
{

}

void Throne::incRessource(int type)
{
	if (playerRef) {
		if (type == 2) {
			playerRef->incCrystal(1);
		}
		else if (type == 3) {
			playerRef->incEnergy(1);
		}
		
	}
}

void Throne::draw(unsigned int texture, GLuint shaderprog)
{
	//send material information to the shader 
	//We don't need to do anything special

	Unit::draw(texture, shaderprog);
}

bool Throne::move(Location *, Map *)
{
	return false;
}

bool Throne::collect(Location *, Map*)
{
	return false;
}

bool Throne::build(int x, int y, std::string obj)
{
	return false;
}

Unit * Throne::spawn(std::string baby, glm::vec2 place)
{
	return nullptr;
}

bool Throne::consume(Unit * food)
{
	return false;
}
